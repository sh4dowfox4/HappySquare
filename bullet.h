#ifndef BULLET_H
#define BULLET_H

#include "entity.h"

class Bullet : public Entity {
private:
    Vector2 velocity;
    float lifetime;
    int damage;

public:
    Bullet(Vector2 pos, Vector2 dir, float speed, int dmg, float life);
    
    void update(float deltaTime) override;
    void draw() const override;
    
    bool isOffScreen() const;
    int getDamage() const;
};

#endif // BULLET_H
