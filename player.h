#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "bullet.h" // Нужно для std::vector<Bullet>&

class Player : public Entity {
private:
    float speed;
    float shootCD;
    float shootTimer;
    int damage;

public:
    Player(Vector2 startPos);
    
    void update(float deltaTime) override;
    void draw() const override;
    
    void shoot(std::vector<Bullet>& bullets, Camera2D& camera);
};

#endif // PLAYER_H
