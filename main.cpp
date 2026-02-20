#include <raylib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>


// 42
struct Bullet {
	Vector2 pos;
	Vector2 dir;
	float speed;
	bool active;

	Bullet(Vector2 p, Vector2 d, float s) : pos(p), dir(d), speed(s), active(true) {}
};

struct Enemy {
	Rectangle rect;
	float speed;
	int hp;
	bool active;
	
	Enemy(Rectangle r, float s, int h) : rect(r), speed(s), hp(h), active(true) {}
};

struct Particle {
	Vector2 pos;
	Vector2 dir;
	float life;
	bool active;
	Color color;

	Particle(Vector2 p, Vector2 d, float l, Color c) : pos(p), dir(d), life(l), active(true), color(c) {}
};

int main() {

	const int ScreenWidth = 1000;
	const int ScreenHeight = 800;

	InitWindow(ScreenWidth, ScreenHeight, "HappySquare");
	SetTargetFPS(60);
	srand(time(NULL));

	Rectangle player = {400, 300, 50, 50};
	float speed = 300.0f;
	int playerHp = 150;
	int score = 0;
	int max_score = 0;

	std::vector<Bullet> bullets;
	std::vector<Enemy> enemies;
	std::vector<Particle> particles;

	float spawnTimer = 0.0f;
	float spawnInterval = 1.0f;

	while(!WindowShouldClose()) {
		float dt = GetFrameTime();

		if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) player.y -= speed * dt;
		if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) player.y += speed * dt;
		if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) player.x -= speed * dt;
		if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) player.x += speed * dt;

		if (player.x < 0) player.x = 0;
		if (player.y < 0) player.y = 0;
		if (player.x + player.width > ScreenWidth) player.x = ScreenWidth - player.width;
		if (player.y + player.height > ScreenHeight) player.y = ScreenHeight - player.height;
		

		// Стрельнул в бабку

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 playerCenter = { player.x + player.width / 2, player.y + player.height / 2 }; 
			Vector2 mousePos = GetMousePosition();
			Vector2 direction = { mousePos.x - playerCenter.x, mousePos.y - playerCenter.y };

			float length = sqrtf(direction.x * direction.x + direction.y * direction.y );
			if (length > 0) {
				direction.x = direction.x / length;
				direction.y = direction.y / length;
			} 
			bullets.push_back(Bullet(playerCenter, direction, 900.0f));
		}
		// Спавн бабки

		spawnTimer += dt;
		if (spawnTimer >= spawnInterval) {
			spawnTimer = 0;
			
			int side = rand() % 4;
			float ex, ey;
			
			if (side == 0) { ex = rand() % ScreenWidth; ey = -50; }
			else if (side == 1) { ex = rand() % ScreenWidth; ey = ScreenWidth + 50; }
			else if (side == 2) { ex = -50; ey = rand() % ScreenHeight; }
			else { ex = ScreenWidth + 50; ey = rand() % ScreenHeight; }

			Rectangle enemyRect = { ex, ey, 40, 40 };
			float enemySpeed = 100.0f + (score * 0.5f);
			enemies.push_back(Enemy(enemyRect, enemySpeed, 3));
		}

		for (auto& b : bullets) {
			if (b.active) {
				b.pos.x += b.dir.x * b.speed * dt;
				b.pos.y += b.dir.y * b.speed * dt;

				if (b.pos.x < 0 || b.pos.x > ScreenWidth ||
					b.pos.y < 0 || b.pos.y > ScreenHeight) {
					b.active = false;
				}
			}
		}
		
		for (auto& e : enemies) {
			if (e.active) {
				Vector2 playerCenter = { player.x + player.width / 2, player.y + player.height / 2 };
				Vector2 enemyCenter = { e.rect.x + e.rect.width / 2, e.rect.y + e.rect.height / 2 };

				Vector2 dir = { playerCenter.x - enemyCenter.x, playerCenter.y - enemyCenter.y};
				float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
				if (len > 0) {
					dir.x /= len;
					dir.y /= len;
				}
				e.rect.x += dir.x * e.speed * dt;
				e.rect.y += dir.y * e.speed * dt;

				if (CheckCollisionRecs(player, e.rect)) {
					e.active = false;
					playerHp -= 20;
					for (int i = 0; i < 10; i++) {
						particles.push_back(Particle(enemyCenter, { (float)(rand()%100-50)/50, (float)(rand()%100-50)/50 }, 0.5f, RED));
					}
				}
			}
		}
		
		for (auto& b : bullets) {
			if (!b.active) continue;
			for (auto& e : enemies) {
				if (e.active && CheckCollisionPointRec(b.pos, e.rect)) {
				b.active = false;
				e.hp -= 100;
					if (e.hp <= 0) {
						e.active = false;
						score += 10;
					
						Vector2 center = { e.rect.x + e.rect.width/2, e.rect.y + e.rect.height/2};
						for(int i = 0; i < 15; i++) {
							particles.push_back(Particle(center, { (float)(rand()%100-50)/50, (float)(rand()%100-50)/50}, 0.7f, ORANGE));
						}
					}
					break;
				}
			}
		}	
		
		for (auto& p : particles) {
			if (p.active) {
				p.pos.x += p.dir.x * 100 * dt;
				p.pos.y += p.dir.y * 100 * dt;
				p.life -= dt;
				if (p.life <= 0) p.active = false;
			}
		}

		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b){ return !b.active; }), bullets.end());
		enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e){ return !e.active; }), enemies.end());
		particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p){ return !p.active; }), particles.end());

		BeginDrawing();

			ClearBackground(GRAY);
			DrawRectangleRec(player, GREEN);

			for (const auto& b : bullets) DrawCircleV(b.pos, 5, YELLOW);
			for (const auto& e : enemies) DrawRectangleRec(e.rect, RED);
			for (const auto& p : particles) DrawPixelV(p.pos, p.color);
				
			DrawText(("Score: " + std::to_string(score)).c_str(), 10, ScreenHeight - 35, 30, WHITE);
			DrawText(("HP: " + std::to_string(playerHp)).c_str(), 10, 10, 35, RED);

			if (playerHp <= 0) {
				max_score = score;
				DrawText("YOU LOSE!", ScreenWidth/2 - 130, ScreenHeight/2 - 100, 60, RED);
				DrawText("Press R to restart", ScreenWidth/2 - 75, ScreenHeight / 2 - 20, 20, WHITE);
				DrawText(("High Score: " + std::to_string(max_score)).c_str(), ScreenWidth/2 - 80, ScreenHeight/2 + 100, 35, WHITE);
					
				if (IsKeyPressed(KEY_R)) {
					playerHp = 100;
					score = 0;
					enemies.clear();
					bullets.clear();
					particles.clear();
				}
			}
		EndDrawing();
	}
	
	CloseWindow();
	return 0;

}

