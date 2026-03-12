#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"

class Player;

class Enemy : public Entity {
public:
    enum class Behavior {
        CHASE,
        IDLE
    };

    enum class State {
        IDLE,   // Спокоен
        ALERTED // Замечает игрока
    };

private:
    float speed;
    int damage;
    float attackCD;
    float attackTimer;
    Behavior behavior;
    
    float alertRadius;  // Малый радиус (первичное обнаружение)
    float chaseRadius;  // Большой радиус (дальность преследования)
    State state;        // Текущее состояние

public:
    // Конструктор теперь принимает оба радиуса
    Enemy(Vector2 pos, float w, float h, int hp, int dmg, float spd, 
          float alertRad, float chaseRad, Behavior beh);
    
    void update(float deltaTime, const Player& player);
    void draw() const override;
    
    bool canDetectPlayer(const Player& player) const;
    void chasePlayer(const Player& player, float deltaTime);
    void attack(Player& player);
    
    State getState() const { return state; }
};

#endif // ENEMY_H
