#ifndef BULLET_H
#define BULLET_H
#include "entity.h"

enum class BulletOwner { PLAYER, ENEMY };

class Bullet : public Entity {
private:
    Vector2 velocity;
    float lifetime;
    int damage;
    BulletOwner owner;

public:
    Bullet(Vector2 pos, Vector2 dir, float speed, int dmg, float life, BulletOwner own);
    void update(float deltaTime) override;
    void draw() const override;
    bool isOffScreen() const;
    int getDamage() const;
    BulletOwner getOwner() const { return owner; }
};
#endif // BULLET_H
