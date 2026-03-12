#include "entity.h"

Entity::Entity(Vector2 p, float w, float h, int hp, Color c)
    : pos(p), width(w), height(h), health(hp), maxHealth(hp), color(c), isActive(true) {}

void Entity::update(float deltaTime) {
    // Базовая реализация пустая
}

void Entity::draw() const {
    DrawRectangle(pos.x, pos.y, width, height, color);
}

bool Entity::checkCollision(const Entity& other) const {
    return (pos.x < other.pos.x + other.width &&
            pos.x + width > other.pos.x &&
            pos.y < other.pos.y + other.height &&
            pos.y + height > other.pos.y);
}

void Entity::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) isActive = false;
}

bool Entity::isAlive() const {
    return health > 0 && isActive;
}

Rectangle Entity::getBounds() const {
    return {pos.x, pos.y, width, height};
}

Vector2 Entity::getPosition() const {
    return pos;
}

int Entity::getHealth() const { return health; }
int Entity::getMaxHealth() const { return maxHealth; }
bool Entity::getIsActive() const { return isActive; }
void Entity::setIsActive(bool active) { isActive = active; }
