#include "game.h"
#include <cstdio>

Game::Game()
: player(Vector2{WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f}),
currentState(GameState::MENU),
previousState(GameState::MENU),
isRunning(true),
currentResIndex(0),
currentMonitorIndex(0),
musicVolume(0.5f),
sfxVolume(0.5f),
hoveredButton(-1),
settingsOpenedFromGame(false),
audioInitialized(false),
debugMode(false),
logoLoaded(false) {
    enemySpawnTimer = 0;
    enemySpawnInterval = 2.f;
    score = 0;
    wave = 1;
    highScore = 0;
    camera.target = player.getPosition();
    camera.offset = {(float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    targetZoom = 1.0f;
    zoomSpeed = 5.0f;
    
    resolutions.push_back({"1280 x 720 (Window)", 1280, 720, false});
    resolutions.push_back({"1920 x 1080 (Window)", 1920, 1080, false});
    resolutions.push_back({"Native Fullscreen", 0, 0, true});
    
    detectMonitors();
    applyResolution(currentResIndex, currentMonitorIndex);
    loadHighScore();
    initAudio();

    menuLogoTexture = LoadTexture("resources/menu_logo.png");
    if (menuLogoTexture.id != 0) {
        logoLoaded = true;
    }
}

Game::~Game() {
    if (logoLoaded) {
        UnloadTexture(menuLogoTexture);
    }
    unloadAudio();
}

void Game::loadHighScore() {
    FILE* f = fopen(HIGHSCORE_FILE, "rb");
    if (f) {
        fread(&highScore, sizeof(int), 1, f);
        fclose(f);
    } else {
        highScore = 0;
    }
}

void Game::saveHighScore() {
    if (score > highScore) {
        highScore = score;
        FILE* f = fopen(HIGHSCORE_FILE, "wb");
        if (f) {
            fwrite(&highScore, sizeof(int), 1, f);
            fclose(f);
        }
    }
}

void Game::initAudio() {
    InitAudioDevice();
    musicMenu = LoadMusicStream("resources/music_menu.mp3");
    musicGame = LoadMusicStream("resources/music_game.mp3");
    musicMenu.looping = true;
    musicGame.looping = true;
    
    sfxShoot = LoadSound("resources/shoot.wav");
    sfxHit = LoadSound("resources/hit.wav");
    sfxKill = LoadSound("resources/kill.wav");
    sfxPlayerHit = LoadSound("resources/player_hit.wav");
    sfxReload = LoadSound("resources/reload.wav");
    sfxEnemyShoot = LoadSound("resources/shoot.wav");

    SetMusicVolume(musicMenu, musicVolume);
    SetMusicVolume(musicGame, musicVolume);
    SetSoundVolume(sfxShoot, sfxVolume);
    SetSoundVolume(sfxHit, sfxVolume);
    SetSoundVolume(sfxKill, sfxVolume);
    SetSoundVolume(sfxPlayerHit, sfxVolume);
    SetSoundVolume(sfxReload, sfxVolume);
    SetSoundVolume(sfxEnemyShoot, sfxVolume);
    
    audioInitialized = true;
    PlayMusicStream(musicMenu);
    
    player.setReloadSound(sfxReload);
}

void Game::updateAudio() {
    if (!audioInitialized) return;
    UpdateMusicStream(musicMenu);
    UpdateMusicStream(musicGame);
    
    SetMusicVolume(musicMenu, musicVolume);
    SetMusicVolume(musicGame, musicVolume);
    SetSoundVolume(sfxShoot, sfxVolume);
    SetSoundVolume(sfxHit, sfxVolume);
    SetSoundVolume(sfxKill, sfxVolume);
    SetSoundVolume(sfxPlayerHit, sfxVolume);
    SetSoundVolume(sfxReload, sfxVolume);
    SetSoundVolume(sfxEnemyShoot, sfxVolume);
}

void Game::switchMusic(GameState state) {
    if (!audioInitialized) return;
    
    if (state == GameState::MENU || state == GameState::GAMEOVER) {
        StopMusicStream(musicGame);
        PlayMusicStream(musicMenu);
    }
    else if (state == GameState::PLAYING || state == GameState::PAUSED) {
        StopMusicStream(musicMenu);
        PlayMusicStream(musicGame);
    }
    else if (state == GameState::SETTINGS) {
        if (previousState == GameState::MENU || previousState == GameState::SETTINGS || previousState == GameState::GAMEOVER) {
            StopMusicStream(musicGame);
            PlayMusicStream(musicMenu);
        }
        else {
            StopMusicStream(musicMenu);
            PlayMusicStream(musicGame);
        }
    }
    else {
        StopMusicStream(musicMenu);
        StopMusicStream(musicGame);
    }
}

void Game::unloadAudio() {
    if (!audioInitialized) return;
    UnloadMusicStream(musicMenu);
    UnloadMusicStream(musicGame);
    UnloadSound(sfxShoot);
    UnloadSound(sfxHit);
    UnloadSound(sfxKill);
    UnloadSound(sfxPlayerHit);
    UnloadSound(sfxReload);
    UnloadSound(sfxEnemyShoot);
    CloseAudioDevice();
    audioInitialized = false;
}

void Game::playShoot() { if (audioInitialized) PlaySound(sfxShoot); }
void Game::playHit() { if (audioInitialized) PlaySound(sfxHit); }
void Game::playKill() { if (audioInitialized) PlaySound(sfxKill); }
void Game::playPlayerHit() { if (audioInitialized) PlaySound(sfxPlayerHit); }
void Game::playReload() { if (audioInitialized) PlaySound(sfxReload); }
void Game::playEnemyShoot() { if (audioInitialized) PlaySound(sfxEnemyShoot); }

void Game::detectMonitors() {}

void Game::applyResolution(int resIndex, int monitorIndex) {
    if (resIndex < 0 || resIndex >= (int)resolutions.size()) return;
    
    ResolutionPreset res = resolutions[resIndex];
    int maxMonitors = GetMonitorCount();
    if (monitorIndex >= maxMonitors) monitorIndex = 0;
    if (monitorIndex < 0) monitorIndex = 0;

    if (res.isFullscreen) {
        int monWidth = GetMonitorWidth(monitorIndex);
        int monHeight = GetMonitorHeight(monitorIndex);
        Vector2 monPos = GetMonitorPosition(monitorIndex);
        
        SetWindowPosition((int)monPos.x, (int)monPos.y);
        if (!IsWindowFullscreen()) ToggleFullscreen();
        SetWindowSize(monWidth, monHeight);
        
        camera.offset = {(float)monWidth / 2.0f, (float)monHeight / 2.0f};
    }
    else {
        if (IsWindowFullscreen()) ToggleFullscreen();
        SetWindowSize(res.width, res.height);
        
        Vector2 monPos = GetMonitorPosition(monitorIndex);
        int monWidth = GetMonitorWidth(monitorIndex);
        int monHeight = GetMonitorHeight(monitorIndex);
        
        int xPos = (int)monPos.x + (monWidth - res.width) / 2;
        int yPos = (int)monPos.y + (monHeight - res.height) / 2;
        SetWindowPosition(xPos, yPos);
        
        camera.offset = {(float)res.width / 2.0f, (float)res.height / 2.0f};
    }
    camera.target = player.getPosition();
}

void Game::run() {
    while (isRunning && !WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        update(deltaTime);
        draw();
    }
}

void Game::update(float deltaTime) {
    updateAudio();
    
    static GameState lastState = GameState::MENU;
    if (currentState != lastState) {
        switchMusic(currentState);
        lastState = currentState;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        if (currentState == GameState::SETTINGS) {
            currentState = previousState;
            return;
        }
        if (currentState == GameState::PAUSED) {
            currentState = GameState::PLAYING;
            return;
        }
        if (currentState == GameState::GAMEOVER) {
            currentState = GameState::MENU;
            return;
        }
        if (currentState == GameState::PLAYING) {
            previousState = GameState::PLAYING;
            currentState = GameState::PAUSED;
            return;
        }
    }

    switch (currentState) {
        case GameState::MENU: updateMenu(deltaTime); break;
        case GameState::SETTINGS: updateSettings(deltaTime); break;
        case GameState::PLAYING: updateGameplay(deltaTime); break;
        case GameState::PAUSED: updatePaused(deltaTime); break;
        case GameState::GAMEOVER: updateGameOver(deltaTime); break;
        case GameState::EXIT: isRunning = false; break;
    }
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    switch (currentState) {
        case GameState::MENU: drawMenu(); break;
        case GameState::SETTINGS: drawSettings(); break;
        case GameState::PLAYING: drawGameplay(); break;
        case GameState::PAUSED: drawGameplay(); drawPaused(); break;
        case GameState::GAMEOVER: drawGameplay(); drawGameOver(); break;
        default: break;
    }
    
    EndDrawing();
}

void Game::updateGameplay(float deltaTime) {
    // Логика игры
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        targetZoom += wheel * 0.1f;
        targetZoom = Clamp(targetZoom, 0.1f, 5.0f);
    }
    camera.zoom = Lerp(camera.zoom, targetZoom, zoomSpeed * deltaTime);
    camera.target = player.getPosition();

    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer >= enemySpawnInterval) {
        spawnEnemy();
        enemySpawnTimer = 0;
    }

    player.update(deltaTime);
    player.shoot(bullets, camera, sfxShoot, sfxReload);

    for (auto& bullet : bullets) bullet.update(deltaTime);
    
    for (auto& enemy : enemies) {
        enemy.update(deltaTime, player);
        if (enemy.attack(player)) {
            playPlayerHit();
        }
        if (enemy.getBehavior() == Enemy::Behavior::RANGER) {
            enemy.shoot(bullets, player, sfxEnemyShoot);
        }
    }

    checkCollision();
    cleanUp();

    // ПЕРЕХОД В GAME OVER ПРИ СМЕРТИ
    if (!player.isAlive()) {
        saveHighScore();
        previousState = GameState::GAMEOVER;
        currentState = GameState::GAMEOVER;
        // Счет и волну не сбрасываем, чтобы показать на экране
    }

    // Обработка кнопки ПАУЗЫ в игре
    int sw = GetScreenWidth();
    Rectangle btnPause = {(float)sw - 160, 10, 150, 40};
    bool hoverPause = CheckCollisionPointRec(GetMousePosition(), btnPause);
    
    if (hoverPause && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        previousState = GameState::PLAYING;
        currentState = GameState::PAUSED;
    }
}

void Game::drawGameplay() {
    BeginMode2D(camera);
    
    DrawRectangleLines(0, 0, WORLD_WIDTH, WORLD_HEIGHT, WHITE);
    for (int x = 0; x <= WORLD_WIDTH; x += 100) DrawLine(x, 0, x, WORLD_HEIGHT, DARKGRAY);
    for (int y = 0; y <= WORLD_HEIGHT; y += 100) DrawLine(0, y, WORLD_WIDTH, y, DARKGRAY);
    
    player.draw();
    for (const auto& enemy : enemies) enemy.draw();
    for (const auto& bullet : bullets) bullet.draw();
    
    EndMode2D();

    // UI
    DrawRectangle(10, 10, 200, 20, WHITE);
    float hpPercent = (float)player.getHealth() / player.getMaxHealth();
    DrawRectangle(10, 10, 200 * hpPercent, 20, GREEN);
    
    char ammoText[50];
    if (player.isReloadingNow()) {
        sprintf(ammoText, "RELOADING... %d/%d", player.getCurrentAmmo(), player.getMagazineSize());
        DrawText(ammoText, 10, 40, 20, RED);
    }
    else {
        sprintf(ammoText, "Ammo: %d/%d", player.getCurrentAmmo(), player.getMagazineSize());
        DrawText(ammoText, 10, 40, 20, YELLOW);
    }
    
    DrawText(TextFormat("Score: %d", score), 10, 70, 20, WHITE);
    DrawText(TextFormat("Wave: %d", wave), 10, 100, 20, WHITE);
    
    if (debugMode) {
        DrawText(TextFormat("DEBUG: Enemies: %d", (int)enemies.size()), 10, 160, 20, YELLOW);
        DrawText(TextFormat("DEBUG: Bullets: %d", (int)bullets.size()), 10, 180, 20, YELLOW);
    }

    // КНОПКА ПАУЗЫ
    int sw = GetScreenWidth();
    Rectangle btnPause = {(float)sw - 160, 10, 150, 40};
    bool hoverPause = CheckCollisionPointRec(GetMousePosition(), btnPause);
    
    DrawRectangleRec(btnPause, Fade(BLACK, 0.5f));
    DrawRectangleLinesEx(btnPause, 2, hoverPause ? WHITE : BLUE);
    DrawText("PAUSE", btnPause.x + 50, btnPause.y + 12, 20, hoverPause ? WHITE : BLUE);
}

void Game::checkCollision() {
    for (auto& bullet : bullets) {
        if (!bullet.getIsActive()) continue;
        
        if (bullet.getOwner() == BulletOwner::PLAYER) {
            for (auto& enemy : enemies) {
                if (!enemy.getIsActive()) continue;
                if (bullet.checkCollision(enemy)) {
                    enemy.takeDamage(bullet.getDamage());
                    bullet.setIsActive(false);
                    enemy.setState(Enemy::State::ALERTED);
                    playHit();
                    if (!enemy.getIsActive()) {
                        score += 10;
                        playKill();
                    }
                    break;
                }
            }
        }
        else if (bullet.getOwner() == BulletOwner::ENEMY) {
            if (bullet.checkCollision(player)) {
                player.takeDamage(bullet.getDamage());
                bullet.setIsActive(false);
                playPlayerHit();
            }
        }
    }
}

void Game::spawnEnemy() {
    if (enemies.size() >= MAX_ENEMIES) return;
    
    float x = (float)GetRandomValue(0, WORLD_WIDTH - 32);
    float y = (float)GetRandomValue(0, WORLD_HEIGHT - 32);
    Vector2 pos = {x, y};
    
    bool isRanger = (GetRandomValue(1, 100) <= 30);
    int hp = 45 + (wave * 5);
    int dmg = 15 + wave;
    float spd = 90.f + (wave * 5);
    
    if (isRanger) {
        enemies.emplace_back(pos, 32, 32, hp, dmg, spd * 0.8f,
                             500.0f, 800.0f, Enemy::Behavior::RANGER);
    }
    else {
        enemies.emplace_back(pos, 32, 32, hp, dmg * 2, spd * 1.5f,
                             900.0f, 1000.0f, Enemy::Behavior::CHASE);
    }
    
    if (score >= wave * 50) {
        wave++;
        enemySpawnInterval = fmaxf(0.5f, enemySpawnInterval - 0.1f);
    }
}

void Game::cleanUp() {
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) {
        return !b.getIsActive() || b.isOffScreen();
    }), bullets.end());
    
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) {
        return !e.getIsActive();
    }), enemies.end());
}

void Game::updateMenu(float deltaTime) {
    int sw = GetScreenWidth();
    int menuStartX = (int)(sw * 0.75f);
    
    Rectangle btnPlay = {(float)menuStartX - 100, 200, 200, 50};
    Rectangle btnSettings = {(float)menuStartX - 100, 270, 200, 50};
    Rectangle btnExit = {(float)menuStartX - 100, 340, 200, 50};
    
    if (CheckCollisionPointRec(GetMousePosition(), btnPlay)) hoveredButton = 0;
    else if (CheckCollisionPointRec(GetMousePosition(), btnSettings)) hoveredButton = 1;
    else if (CheckCollisionPointRec(GetMousePosition(), btnExit)) hoveredButton = 2;
    else hoveredButton = -1;
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (hoveredButton == 0) {
            score = 0;
            wave = 1;
            enemySpawnInterval = 2.0f;
            player = Player(Vector2{WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f});
            player.setReloadSound(sfxReload);
            enemies.clear();
            bullets.clear();
            camera.target = player.getPosition();
            currentState = GameState::PLAYING;
        }
        else if (hoveredButton == 1) {
            previousState = GameState::MENU;
            settingsOpenedFromGame = false;
            currentState = GameState::SETTINGS;
        }
        else if (hoveredButton == 2) {
            currentState = GameState::EXIT;
        }
    }
}

void Game::drawMenu() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int menuStartX = (int)(sw * 0.75f);

    // Отрисовка логотипа слева
    float logoAreaWidth = sw * 0.6f;
    
    if (logoLoaded) {
        float maxW = logoAreaWidth * 0.9f;
        float maxH = sh * 0.8f;
        float scale = fminf(maxW / menuLogoTexture.width, maxH / menuLogoTexture.height);
        
        float w = menuLogoTexture.width * scale;
        float h = menuLogoTexture.height * scale;
        
        float posX = (logoAreaWidth - w) / 2.0f;
        float posY = (sh - h) / 2.0f;
        
        DrawTextureEx(menuLogoTexture, {posX, posY}, 0.0f, scale, WHITE);
    }
    else {
        float centerX = logoAreaWidth / 2.0f;
        float centerY = sh / 2.0f;
        float size = 200.0f;
        
        DrawRectangle(0, 0, (int)logoAreaWidth, sh, Fade(DARKGRAY, 0.1f));
        
        Rectangle logoRect = {centerX - size/2, centerY - size/2, size, size};
        DrawRectangleRec((Rectangle){logoRect.x - 5, logoRect.y - 5, logoRect.width + 10, logoRect.height + 10}, Fade(GREEN, 0.3f));
        DrawRectangleRec(logoRect, GREEN);
        DrawRectangleLinesEx(logoRect, 4, WHITE);
        
        DrawText("HAPPY", (int)(centerX - MeasureText("HAPPY", 40)/2), (int)(centerY - 45), 40, BLACK);
        DrawText("SQUARE", (int)(centerX - MeasureText("SQUARE", 40)/2), (int)(centerY + 5), 40, BLACK);
    }

    DrawText("HappySquare", menuStartX - MeasureText("HappySquare", 60)/2, 80, 60, BLUE);
    DrawText(TextFormat("High Score: %d", highScore), menuStartX - MeasureText("High Score: 999999", 30)/2, 150, 30, GOLD);
    
    Rectangle btnPlay = {(float)menuStartX - 100, 200, 200, 50};
    Rectangle btnSettings = {(float)menuStartX - 100, 270, 200, 50};
    Rectangle btnExit = {(float)menuStartX - 100, 340, 200, 50};
    
    drawNeonButton(btnPlay, "PLAY", hoveredButton == 0, BLUE);
    drawNeonButton(btnSettings, "SETTINGS", hoveredButton == 1, BLUE);
    drawNeonButton(btnExit, "EXIT", hoveredButton == 2, RED);
}

// --- ФУНКЦИИ GAME OVER ---

void Game::updateGameOver(float deltaTime) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    
    int centerY = sh / 2;
    int buttonStartY = centerY + 60; // Фиксированный отступ

    Rectangle btnNewGame = {(float)sw/2 - 100, (float)buttonStartY, 200, 50};
    Rectangle btnMainMenu = {(float)sw/2 - 100, (float)(buttonStartY + 70), 200, 50};
    
    if (CheckCollisionPointRec(GetMousePosition(), btnNewGame)) hoveredButton = 0;
    else if (CheckCollisionPointRec(GetMousePosition(), btnMainMenu)) hoveredButton = 1;
    else hoveredButton = -1;
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (hoveredButton == 0) {
            // Новая игра
            score = 0;
            wave = 1;
            enemySpawnInterval = 2.0f;
            player = Player(Vector2{WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f});
            player.setReloadSound(sfxReload);
            enemies.clear();
            bullets.clear();
            camera.target = player.getPosition();
            currentState = GameState::PLAYING;
        }
        else if (hoveredButton == 1) {
            // В главное меню
            currentState = GameState::MENU;
        }
    }
}

void Game::drawGameOver() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    
    // Затемнение
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.85f));
    
    int centerY = sh / 2;

    // Заголовок
    const char* goText = "GAME OVER";
    int goFontSize = 60;
    int goWidth = MeasureText(goText, goFontSize);
    DrawText(goText, sw/2 - goWidth/2, centerY - 100, goFontSize, RED);
    
    // Статистика
    char scoreText[64];
    sprintf(scoreText, "Final Score: %d", score);
    int stFontSize = 30;
    int stWidth = MeasureText(scoreText, stFontSize);
    DrawText(scoreText, sw/2 - stWidth/2, centerY - 30, stFontSize, WHITE);
    
    char waveText[64];
    sprintf(waveText, "Wave Reached: %d", wave);
    int wtFontSize = 25;
    int wtWidth = MeasureText(waveText, wtFontSize);
    DrawText(waveText, sw/2 - wtWidth/2, centerY + 5, wtFontSize, GRAY);

    // Кнопки (строго ниже текста)
    int buttonStartY = centerY + 60; 
    Rectangle btnNewGame = {(float)sw/2 - 100, (float)buttonStartY, 200, 50};
    Rectangle btnMainMenu = {(float)sw/2 - 100, (float)(buttonStartY + 70), 200, 50};
    
    drawNeonButton(btnNewGame, "NEW GAME", hoveredButton == 0, GREEN);
    drawNeonButton(btnMainMenu, "MAIN MENU", hoveredButton == 1, BLUE);
}

// ---------------------------

void Game::updatePaused(float deltaTime) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float startY = sh / 2.0f - 90; // Сдвинул вниз для 4 кнопок
    
    Rectangle btnResume = {(float)sw/2 - 100, startY, 200, 50};
    Rectangle btnSettings = {(float)sw/2 - 100, startY + 70, 200, 50};
    Rectangle btnMainMenu = {(float)sw/2 - 100, startY + 140, 200, 50};
    Rectangle btnExitGame = {(float)sw/2 - 100, startY + 210, 200, 50};
    
    if (CheckCollisionPointRec(GetMousePosition(), btnResume)) hoveredButton = 0;
    else if (CheckCollisionPointRec(GetMousePosition(), btnSettings)) hoveredButton = 1;
    else if (CheckCollisionPointRec(GetMousePosition(), btnMainMenu)) hoveredButton = 2;
    else if (CheckCollisionPointRec(GetMousePosition(), btnExitGame)) hoveredButton = 3;
    else hoveredButton = -1;
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (hoveredButton == 0) {
            currentState = GameState::PLAYING;
        }
        else if (hoveredButton == 1) {
            previousState = GameState::PAUSED;
            settingsOpenedFromGame = true;
            currentState = GameState::SETTINGS;
        }
        else if (hoveredButton == 2) {
            saveHighScore();
            score = 0;
            wave = 1;
            enemySpawnInterval = 2.0f;
            player = Player(Vector2{WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f});
            player.setReloadSound(sfxReload);
            enemies.clear();
            bullets.clear();
            camera.target = player.getPosition();
            currentState = GameState::MENU;
        }
        else if (hoveredButton == 3) {
            currentState = GameState::EXIT;
        }
    }
}

void Game::drawPaused() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.7f));
    DrawText("PAUSED", sw/2 - MeasureText("PAUSED", 50)/2, sh/2 - 150, 50, BLUE);
    
    float startY = sh / 2.0f - 90;
    Rectangle btnResume = {(float)sw/2 - 100, startY, 200, 50};
    Rectangle btnSettings = {(float)sw/2 - 100, startY + 70, 200, 50};
    Rectangle btnMainMenu = {(float)sw/2 - 100, startY + 140, 200, 50};
    Rectangle btnExitGame = {(float)sw/2 - 100, startY + 210, 200, 50};
    
    drawNeonButton(btnResume, "RESUME", hoveredButton == 0, BLUE);
    drawNeonButton(btnSettings, "SETTINGS", hoveredButton == 1, BLUE);
    drawNeonButton(btnMainMenu, "MAIN MENU", hoveredButton == 2, BLUE);
    drawNeonButton(btnExitGame, "EXIT", hoveredButton == 3, RED);
}

void Game::updateSettings(float deltaTime) {
    int sw = GetScreenWidth();
    
    Rectangle btnBack = {(float)sw/2 - 100, 650, 200, 50};
    Rectangle btnDebug = {(float)sw/2 - 100, 500, 200, 50};
    
    bool hoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    bool hoverDebug = CheckCollisionPointRec(GetMousePosition(), btnDebug);
    
    if (hoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        applyResolution(currentResIndex, currentMonitorIndex);
        currentState = previousState;
        return;
    }
    
    if (hoverDebug && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        debugMode = !debugMode;
        g_debugMode = debugMode;
    }
    
    Rectangle rectRes = {(float)sw/2 - 150, 150, 300, 40};
    if (CheckCollisionPointRec(GetMousePosition(), rectRes) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        currentResIndex = (currentResIndex + 1) % resolutions.size();
        applyResolution(currentResIndex, currentMonitorIndex);
    }
    
    int monitorCount = GetMonitorCount();
    Rectangle rectMon = {(float)sw/2 - 150, 220, 300, 40};
    if (monitorCount > 1) {
        if (CheckCollisionPointRec(GetMousePosition(), rectMon) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentMonitorIndex = (currentMonitorIndex + 1) % monitorCount;
            applyResolution(currentResIndex, currentMonitorIndex);
        }
    }
    
    Rectangle rectMusic = {(float)sw/2 - 100, 360, 200, 20};
    if (CheckCollisionPointRec(GetMousePosition(), rectMusic) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        musicVolume = Clamp((GetMousePosition().x - rectMusic.x) / rectMusic.width, 0.0f, 1.0f);
    }
    
    Rectangle rectSfx = {(float)sw/2 - 100, 420, 200, 20};
    if (CheckCollisionPointRec(GetMousePosition(), rectSfx) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        sfxVolume = Clamp((GetMousePosition().x - rectSfx.x) / rectSfx.width, 0.0f, 1.0f);
    }
}

void Game::drawSettings() {
    int sw = GetScreenWidth();
    
    DrawText("SETTINGS", sw/2 - MeasureText("SETTINGS", 40)/2, 50, 40, BLUE);
    
    DrawText("Resolution:", sw/2 - 150, 120, 20, WHITE);
    const char* resName = resolutions[currentResIndex].name;
    Rectangle rectRes = {(float)sw/2 - 150, 150, 300, 40};
    bool hoverRes = CheckCollisionPointRec(GetMousePosition(), rectRes);
    DrawRectangleLinesEx(rectRes, 2, hoverRes ? WHITE : BLUE);
    DrawText(resName, rectRes.x + (rectRes.width - MeasureText(resName, 20))/2, rectRes.y + 10, 20, hoverRes ? WHITE : BLUE);
    
    int monitorCount = GetMonitorCount();
    if (monitorCount > 1) {
        DrawText("Monitor:", sw/2 - 150, 190, 20, WHITE);
        const char* monName = TextFormat("Monitor %d", currentMonitorIndex + 1);
        Rectangle rectMon = {(float)sw/2 - 150, 220, 300, 40};
        bool hoverMon = CheckCollisionPointRec(GetMousePosition(), rectMon);
        DrawRectangleLinesEx(rectMon, 2, hoverMon ? WHITE : BLUE);
        DrawText(monName, rectMon.x + (rectMon.width - MeasureText(monName, 20))/2, rectMon.y + 10, 20, hoverMon ? WHITE : BLUE);
    }
    
    drawSlider({(float)sw/2 - 100, 360, 200, 20}, musicVolume, "Music Volume");
    drawSlider({(float)sw/2 - 100, 420, 200, 20}, sfxVolume, "SFX Volume");
    
    Rectangle btnDebug = {(float)sw/2 - 100, 500, 200, 50};
    bool hoverDebug = CheckCollisionPointRec(GetMousePosition(), btnDebug);
    Color debugColor = debugMode ? LIME : GRAY;
    drawNeonButton(btnDebug, debugMode ? "DEBUG: ON" : "DEBUG: OFF", hoverDebug, debugColor);
    
    Rectangle btnBack = {(float)sw/2 - 100, 650, 200, 50};
    bool hoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    drawNeonButton(btnBack, "BACK", hoverBack, BLUE);
    
    const char* statusText = settingsOpenedFromGame ? "In-Game Settings" : "Main Menu Settings";
    DrawText(statusText, sw/2 - MeasureText(statusText, 15)/2, 630, 15, GRAY);
}

void Game::drawSlider(Rectangle rec, float value, const char* label) {
    DrawText(label, rec.x, rec.y - 25, 20, WHITE);
    DrawRectangleLinesEx(rec, 2, BLUE);
    DrawRectangleRec((Rectangle){rec.x, rec.y, rec.width * value, rec.height}, BLUE);
    DrawText(TextFormat("%.0f%%", value * 100), rec.x + rec.width + 10, rec.y, 20, WHITE);
}

void Game::drawNeonButton(Rectangle rec, const char* text, bool isHovered, Color baseColor) {
    Color drawColor = isHovered ? WHITE : baseColor;
    
    if (isHovered) {
        DrawRectangleRec((Rectangle){rec.x-2, rec.y-2, rec.width+4, rec.height+4}, Fade(baseColor, 0.3f));
    }
    
    DrawRectangleLinesEx(rec, 2, drawColor);
    int textWidth = MeasureText(text, 20);
    DrawText(text, rec.x + (rec.width - textWidth)/2, rec.y + 15, 20, drawColor);
}
