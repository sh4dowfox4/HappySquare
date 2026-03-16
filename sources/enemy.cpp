#include "enemy.h"
#include "player.h"

Enemy::Enemy(Vector2 p, float w, float h, int hp, int dmg, float spd, 
             float alertRad, float chaseRad, Behavior beh)
    : Entity(p, w, h, hp, MAROON) {
    damage = dmg;
    speed = spd;
    attackCD = 1.f;
    attackTimer = 0;
    behavior = beh;
    alertRadius = alertRad;
    chaseRadius = chaseRad;
    state = State::IDLE; // Изначально спокоен
}

bool Enemy::canDetectPlayer(const Player& player) const {
    float distance = Vector2Distance(pos, player.getPosition());
    
    if (state == State::ALERTED) {
        // Если уже злой — видит дальше (chaseRadius)
        return distance <= chaseRadius;
    } else {
        // Если спокоен — видит только вблизи (alertRadius)
        return distance <= alertRadius;
    }
}

void Enemy::update(float deltaTime, const Player& player) {
    if (attackTimer > 0) attackTimer -= deltaTime;
    
    // Проверка видимости
    if (canDetectPlayer(player)) {
        state = State::ALERTED; // Запоминаем игрока
        
        switch(behavior) {
            case Behavior::CHASE:
                chasePlayer(player, deltaTime);
                break;
            case Behavior::IDLE:
                break;
        }
    } else {
        // Если игрок ушёл далеко, враг успокаивается
        state = State::IDLE;
    }
}

void Enemy::chasePlayer(const Player& player, float deltaTime) {
    Vector2 toPlayer = {
        player.getPosition().x - pos.x,
        player.getPosition().y - pos.y
    };
    
    float length = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
    if (length > 0) {
        toPlayer.x /= length;
        toPlayer.y /= length;
        pos.x += toPlayer.x * speed * deltaTime;
        pos.y += toPlayer.y * speed * deltaTime;
    }
}

void Enemy::attack(Player& player) {
    // Атакуем только если видим игрока
    if (canDetectPlayer(player) && checkCollision(player) && attackTimer <= 0) {
        player.takeDamage(damage);
        attackTimer = attackCD;
    }
}

void Enemy::draw() const {
    // Меняем цвет в зависимости от состояния
    Color drawColor = (state == State::ALERTED) ? RED : MAROON;
    DrawRectangle(pos.x, pos.y, width, height, drawColor);
    
    // Полоска здоровья
    float hpPercent = (float)health / maxHealth;
    DrawRectangle(pos.x, pos.y - 8, width * hpPercent, 4, GREEN);
    
    // --- ОТЛАДКА: Рисуем радиусы ---
    // Малый радиус (белый)
    DrawCircleLines(pos.x + width/2, pos.y + height/2, alertRadius, WHITE);
    
    // Большой радиус (красный, только если злой)
    if (state == State::ALERTED) {
        DrawCircleLines(pos.x + width/2, pos.y + height/2, chaseRadius, RED);
    }
}
