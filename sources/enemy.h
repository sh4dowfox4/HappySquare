#ifndef ENEMY_H
#define ENEMY_H
#include "entity.h"
#include "bullet.h"

class Player;

class Enemy : public Entity {
public:
    enum class Behavior { CHASE, RANGER };
    enum class State { IDLE, ALERTED };

private:
    float speed;
    int damage;
    float attackCD;
    float attackTimer;
    Behavior behavior;
    float alertRadius;
    float chaseRadius;
    State state;

    // Стрельба для дальников
    int magazineSize;
    int currentAmmo;
    float shootCD;
    float shootTimer;
    float reloadDelay;
    float reloadTime;
    float reloadTimer;
    bool isReloading;

public:
    Enemy(Vector2 pos, float w, float h, int hp, int dmg, float spd,
          float alertRad, float chaseRad, Behavior beh);

    void update(float deltaTime, const Player& player);
    void draw() const override;
    bool canDetectPlayer(const Player& player) const;
    void chasePlayer(const Player& player, float deltaTime);
    bool attack(Player& player);
    bool shoot(std::vector<Bullet>& bullets, const Player& player, Sound& shootSfx);

    State getState() const { return state; }
    void setState(State s) { state = s; }
    Behavior getBehavior() const { return behavior; }
    float getAlertRadius() const { return alertRadius; }
    float getChaseRadius() const { return chaseRadius; }
};
#endif // ENEMY_H
