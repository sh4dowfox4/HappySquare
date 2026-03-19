#ifndef ENTITY_H
#define ENTITY_H

#include "common.h"

class Entity {
protected:
    Vector2 pos;
    float width;
    float height;
    int health;
    int maxHealth;
    Color color;
    bool isActive;

public:
    Entity(Vector2 p, float w, float h, int hp, Color c);
    virtual ~Entity() = default;

    virtual void update(float deltaTime);
    virtual void draw() const;

    bool checkCollision(const Entity& other) const;
    void takeDamage(int damage);
    bool isAlive() const;

    Rectangle getBounds() const;
    Vector2 getPosition() const;
    int getHealth() const;
    int getMaxHealth() const;
    bool getIsActive() const;
    void setIsActive(bool active);
};

#endif // ENTITY_H
