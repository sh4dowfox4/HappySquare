#include "bullet.h"

Bullet::Bullet(Vector2 p, Vector2 dir, float speed, int dmg, float life, BulletOwner own)
    : Entity(p, 4, 8, 1, (own == BulletOwner::PLAYER) ? YELLOW : RED),
      damage(dmg),
      lifetime(life),
      owner(own) {
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length > 0) {
        velocity.x = (dir.x / length) * speed;
        velocity.y = (dir.y / length) * speed;
    } else {
        velocity = {0, 0};
    }
}

void Bullet::update(float deltaTime) {
    pos.x += velocity.x * deltaTime;
    pos.y += velocity.y * deltaTime;
    lifetime -= deltaTime;
    if (lifetime <= 0) {
        setIsActive(false);
    }
}

bool Bullet::isOffScreen() const {
    return (pos.x < 0 || pos.x > WORLD_WIDTH ||
            pos.y < 0 || pos.y > WORLD_HEIGHT);
}

int Bullet::getDamage() const {
    return damage;
}

void Bullet::draw() const {
    DrawRectangle(pos.x, pos.y, width, height, color);
}
