#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"

constexpr int MAX_ENEMIES = 50;

enum class GameState {
    MENU,
    SETTINGS,
    PLAYING,
    EXIT
};

struct ResolutionPreset {
    const char* name;
    int width;
    int height;
    bool isFullscreen;
};

class Game {
private:
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    
    float enemySpawnTimer;
    float enemySpawnInterval;
    int score;
    int wave;
    
    Camera2D camera;
    float targetZoom;
    float zoomSpeed;

    GameState currentState;
    GameState previousState;
    bool isRunning;
    
    // Настройки
    int currentResIndex;
    int currentMonitorIndex; // Индекс выбранного монитора
    float musicVolume;
    float sfxVolume;
    
    std::vector<ResolutionPreset> resolutions;
    int hoveredButton;
    bool settingsOpenedFromGame;

public:
    Game();
    void run();
    
    void update(float deltaTime);
    void draw();

    void updateGameplay(float deltaTime);
    void drawGameplay();
    void checkCollision();
    void spawnEnemy();
    void cleanUp();

    void updateMenu(float deltaTime);
    void drawMenu();
    
    void updateSettings(float deltaTime);
    void drawSettings();
    
    void applyResolution(int resIndex, int monitorIndex);
    void detectMonitors(); // Поиск доступных мониторов
    
    void drawNeonButton(Rectangle rec, const char* text, bool isHovered, Color color);
    void drawSlider(Rectangle rec, float value, const char* label);
};

#endif // GAME_H
