#ifndef GAME_H
#define GAME_H
#include "common.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"

constexpr int MAX_ENEMIES = 50;

// Добавлено состояние GAMEOVER
enum class GameState { MENU, SETTINGS, PLAYING, PAUSED, GAMEOVER, EXIT };

struct ResolutionPreset {
    const char* name;
    int width;
    int height;
    bool isFullscreen;
};

extern bool g_debugMode;

class Game {
private:
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    float enemySpawnTimer;
    float enemySpawnInterval;
    int score;
    int wave;
    int highScore;
    Camera2D camera;
    float targetZoom;
    float zoomSpeed;
    GameState currentState;
    GameState previousState;
    bool isRunning;
    int currentResIndex;
    int currentMonitorIndex;
    bool debugMode;
    float musicVolume;
    float sfxVolume;
    Music musicMenu;
    Music musicGame;
    Sound sfxShoot;
    Sound sfxHit;
    Sound sfxKill;
    Sound sfxPlayerHit;
    Sound sfxReload;
    Sound sfxEnemyShoot;
    bool audioInitialized;
    std::vector<ResolutionPreset> resolutions;
    int hoveredButton;
    bool settingsOpenedFromGame;
    Texture2D menuLogoTexture;
    bool logoLoaded;

    void initAudio();
    void updateAudio();
    void switchMusic(GameState state);
    void unloadAudio();
    void playShoot();
    void playHit();
    void playKill();
    void playPlayerHit();
    void playReload();
    void playEnemyShoot();
    void loadHighScore();
    void saveHighScore();

    // Новые функции для экрана проигрыша
    void updateGameOver(float deltaTime);
    void drawGameOver();

public:
    Game();
    ~Game();
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
    void updatePaused(float deltaTime);
    void drawPaused();
    void updateSettings(float deltaTime);
    void drawSettings();
    void applyResolution(int resIndex, int monitorIndex);
    void detectMonitors();
    void drawNeonButton(Rectangle rec, const char* text, bool isHovered, Color color);
    void drawSlider(Rectangle rec, float value, const char* label);
    bool isDebugMode() const { return debugMode; }
};

extern bool g_debugMode;
#endif // GAME_H
