#include "player.h"

Player::Player(Vector2 startPos) 
    : Entity(startPos, 32, 32, 100, COLOR_GREEN) {
    speed = 200.f;
    shootCD = 0.2f;
    shootTimer = 0;
    damage = 10;
}

void Player::update(float deltaTime) {
    if (shootTimer > 0) shootTimer -= deltaTime;
    
    Vector2 dir = {0, 0};
    if (IsKeyDown(KEY_W)) dir.y -= 1;
    if (IsKeyDown(KEY_S)) dir.y += 1;
    if (IsKeyDown(KEY_A)) dir.x -= 1;
    if (IsKeyDown(KEY_D)) dir.x += 1;
    
    pos.x += dir.x * speed * deltaTime;
    pos.y += dir.y * speed * deltaTime;
    
    // Ограничение картой
    pos.x = Clamp(pos.x, 0, WORLD_WIDTH - width);
    pos.y = Clamp(pos.y, 0, WORLD_HEIGHT - height);
}

void Player::draw() const {
    DrawRectangle(pos.x, pos.y, width, height, color);
}

void Player::shoot(std::vector<Bullet>& bullets, Camera2D& camera) {
    if (shootTimer <= 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        Vector2 center = {pos.x + width / 2, pos.y + height / 2};
        
        Vector2 toMouse = {
            mouseWorldPos.x - center.x,
            mouseWorldPos.y - center.y
        };
        
        float length = sqrt(toMouse.x * toMouse.x + toMouse.y * toMouse.y);
        if (length > 0) {
            toMouse.x /= length;
            toMouse.y /= length;
        }
        
        Vector2 bulletPos = {
            pos.x + width / 2 - 2,
            pos.y + height / 2 - 4
        };
        
        bullets.emplace_back(bulletPos, toMouse, 500.f, damage, 2.f);
        shootTimer = shootCD;
    }
}
