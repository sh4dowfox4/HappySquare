#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"

class Game {
private:
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    
    float enemySpawnTimer;
    float enemySpawnInterval;
    int score;
    int wave;
    bool isRunning;
    
    Camera2D camera;
    float targetZoom;
    float zoomSpeed;

public:
    Game();
    
    void run();
    void update(float deltaTime);
    void draw();
    
    void checkCollision();
    void spawnEnemy();
    void cleanUp();
};

#endif // GAME_H
