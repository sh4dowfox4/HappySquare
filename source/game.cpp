#include "game.h"

Game::Game() : player(Vector2{WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f}) {
    enemySpawnTimer = 0;
    enemySpawnInterval = 2.f;
    score = 0;
    wave = 1;
    isRunning = true;
    
    camera.target = player.getPosition();
    camera.offset = (Vector2){ (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    targetZoom = 1.0f;
    zoomSpeed = 5.0f;
}

void Game::run() {
    while (isRunning && !WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        update(deltaTime);
        draw();
    }
}

void Game::update(float deltaTime) {
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        targetZoom += wheel * 0.1f;
        if (targetZoom < 0.1f) targetZoom = 0.1f;
        if (targetZoom > 5.0f) targetZoom = 5.0f;
    }
    
    camera.zoom = Lerp(camera.zoom, targetZoom, zoomSpeed * deltaTime);
    camera.target = player.getPosition();
    
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer >= enemySpawnInterval) {
        spawnEnemy();
        enemySpawnTimer = 0;
    }
    
    player.update(deltaTime);
    player.shoot(bullets, camera);
    
    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
    }
    
    for (auto& enemy : enemies) {
        enemy.update(deltaTime, player);
        enemy.attack(player);
    }
    
    checkCollision();
    cleanUp();
    
    if (!player.isAlive()) {
        isRunning = false;
    }
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(COLOR_BLACK);
    
    BeginMode2D(camera);
    
    // Границы карты
    DrawRectangleLines(0, 0, WORLD_WIDTH, WORLD_HEIGHT, COLOR_WHITE);
    
    // Сетка
    for (int x = 0; x <= WORLD_WIDTH; x += 100) {
        DrawLine(x, 0, x, WORLD_HEIGHT, COLOR_DARKGRAY);
    }
    for (int y = 0; y <= WORLD_HEIGHT; y += 100) {
        DrawLine(0, y, WORLD_WIDTH, y, COLOR_DARKGRAY);
    }
    
    player.draw();
    for (const auto& enemy : enemies) {
        enemy.draw();
    }
    for (const auto& bullet : bullets) {
        bullet.draw();
    }
    
    EndMode2D();
    
    // UI
    DrawRectangle(10, 10, 200, 20, COLOR_WHITE);
    float hpPercent = (float)player.getHealth() / player.getMaxHealth();
    DrawRectangle(10, 10, 200 * hpPercent, 20, COLOR_GREEN);
    
    DrawText(TextFormat("Score: %d", score), 10, 40, 20, COLOR_WHITE);
    DrawText(TextFormat("Wave: %d", wave), 10, 70, 20, COLOR_WHITE);
    DrawText(TextFormat("Zoom: %.2f", camera.zoom), 10, 100, 20, COLOR_WHITE);
    DrawText(TextFormat("Pos: %.0f, %.0f", player.getPosition().x, player.getPosition().y), 10, 130, 20, COLOR_WHITE);
    
    EndDrawing();
}

void Game::checkCollision() {
    for (auto& bullet : bullets) {
        if (!bullet.getIsActive()) continue;
        for (auto& enemy : enemies) {
            if (!enemy.getIsActive()) continue;
            if (bullet.checkCollision(enemy)) {
                enemy.takeDamage(bullet.getDamage());
                bullet.setIsActive(false);
                if (!enemy.getIsActive()) {
                    score += 10;
                }
                break;
            }
        }
    }
}


void Game::spawnEnemy() {
    int side = GetRandomValue(0, 3);
    float x, y;
    
    // Спавн за пределами экрана
    switch(side) {
        case 0: x = (float)GetRandomValue(0, WORLD_WIDTH - 32); y = -32; break;
        case 1: x = (float)GetRandomValue(0, WORLD_WIDTH - 32); y = WORLD_HEIGHT + 32; break;
        case 2: x = -32; y = (float)GetRandomValue(0, WORLD_HEIGHT - 32); break;
        default: x = WORLD_WIDTH + 32; y = (float)GetRandomValue(0, WORLD_HEIGHT - 32); break;
    }
    
    Vector2 pos = {x, y};
    int hp = 20 + (wave * 5);
    int dmg = 5 + wave;
    float spd = 50.f + (wave * 5);
    
    // <-- НОВЫЙ ПАРАМЕТР: Радиус обнаружения (400 пикселей)
    float detectionRadius = 400.0f; 
    
    Enemy::Behavior beh = Enemy::Behavior::CHASE;
    
    // ВАЖНО: Передаем 8 параметров в конструктор (pos, w, h, hp, dmg, spd, radius, beh)
    enemies.emplace_back(pos, 32, 32, hp, dmg, spd, detectionRadius, beh);
    
    if (score >= wave * 100) {
        wave++;
        enemySpawnInterval = fmaxf(0.5f, enemySpawnInterval - 0.1f);
    }
}

void Game::cleanUp() {
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) {
                return !b.getIsActive() || b.isOffScreen();
            }
        ),
        bullets.end()
    );
    
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) {
                return !e.getIsActive();
            }
        ),
        enemies.end()
    );
}
