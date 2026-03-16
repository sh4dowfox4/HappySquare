#include "game.h"

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
settingsOpenedFromGame(false) {

    enemySpawnTimer = 0;
    enemySpawnInterval = 2.f;
    score = 0;
    wave = 1;

    // Инициализация камеры (важно: оффсет будет обновлен в первом кадре draw/update)
    camera.target = player.getPosition();
    camera.offset = { (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    targetZoom = 1.0f;
    zoomSpeed = 5.0f;

    // Пресеты разрешений
    resolutions.push_back({"1280 x 720 (Window)", 1280, 720, false});
    resolutions.push_back({"1920 x 1080 (Window)", 1920, 1080, false});
    resolutions.push_back({"2560 x 1440 (Window)", 2560, 1440, false});
    resolutions.push_back({"Native Fullscreen", 0, 0, true});

    // Определяем мониторы при старте
    detectMonitors();
    // Применяем настройки по умолчанию
    applyResolution(currentResIndex, currentMonitorIndex);
}

void Game::detectMonitors() {
    int count = GetMonitorCount();
    if (count > 1) {
        // Если мониторов больше 1, можно добавить логику выбора в меню настроек
        // Пока просто оставляем 0 (основной), но переменная готова
    }
}

void Game::applyResolution(int resIndex, int monitorIndex) {
    if (resIndex < 0 || resIndex >= resolutions.size()) return;

    ResolutionPreset res = resolutions[resIndex];

    // Защита от выхода за пределы доступных мониторов
    int maxMonitors = GetMonitorCount();
    if (monitorIndex >= maxMonitors) monitorIndex = 0;
    if (monitorIndex < 0) monitorIndex = 0;

    if (res.isFullscreen) {
        // --- ЛОГИКА FULLSCREEN ---

        // 1. Получаем реальное разрешение выбранного монитора
        int monWidth = GetMonitorWidth(monitorIndex);
        int monHeight = GetMonitorHeight(monitorIndex);

        // 2. Если окно еще не на этом мониторе (было оконным на другом), перемещаем его туда
        // Это важно для корректной работы ToggleFullscreen на нужном экране
        Vector2 monPos = GetMonitorPosition(monitorIndex);
        SetWindowPosition((int)monPos.x, (int)monPos.y);

        // 3. Включаем полноэкранный режим
        if (!IsWindowFullscreen()) {
            ToggleFullscreen();
        }

        // 4. КРИТИЧЕСКИ ВАЖНО: Явно устанавливаем размер окна в разрешение монитора.
        // Иногда ToggleFullscreen сбивает буфер обмена, эта строка фиксирует рендеринг 1-в-1.
        SetWindowSize(monWidth, monHeight);

        // Обновляем оффсет камеры под новый полный размер
        camera.offset = { (float)monWidth / 2.0f, (float)monHeight / 2.0f };

    } else {
        // --- ЛОГИКА ОКОННОГО РЕЖИМА ---

        // Если были в фуллскрине, выходим из него
        if (IsWindowFullscreen()) {
            ToggleFullscreen();
            // Небольшая задержка иногда нужна ОС для применения изменений,
            // но обычно SetWindowSize сразу после этого работает корректно.
        }

        SetWindowSize(res.width, res.height);

        // Центрируем окно на выбранном мониторе
        Vector2 monPos = GetMonitorPosition(monitorIndex);
        int monWidth = GetMonitorWidth(monitorIndex);
        int monHeight = GetMonitorHeight(monitorIndex);

        int xPos = (int)monPos.x + (monWidth - res.width) / 2;
        int yPos = (int)monPos.y + (monHeight - res.height) / 2;

        SetWindowPosition(xPos, yPos);

        // Обновляем оффсет камеры под размер окна
        camera.offset = { (float)res.width / 2.0f, (float)res.height / 2.0f };
    }

    // Возвращаем фокус камеры на игрока, чтобы он не потерялся при смене разрешения
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
    // Глобальный ESC для выхода из настроек
    if (currentState == GameState::SETTINGS && IsKeyPressed(KEY_ESCAPE)) {
        currentState = previousState;
        return;
    }

    switch (currentState) {
        case GameState::MENU:
            updateMenu(deltaTime);
            break;
        case GameState::SETTINGS:
            updateSettings(deltaTime);
            break;
        case GameState::PLAYING:
            updateGameplay(deltaTime);
            break;
        case GameState::EXIT:
            isRunning = false;
            break;
    }
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(COLOR_BLACK);

    switch (currentState) {
        case GameState::MENU:
            drawMenu();
            break;
        case GameState::SETTINGS:
            drawSettings();
            break;
        case GameState::PLAYING:
            drawGameplay();
            break;
        default:
            break;
    }

    EndDrawing();
}

// ================= ИГРОВОЙ ПРОЦЕСС (ИСПРАВЛЕННЫЙ) =================

void Game::updateGameplay(float deltaTime) {
    // ESC -> Меню (не выход!)
    if (IsKeyPressed(KEY_ESCAPE)) {
        previousState = GameState::PLAYING;
        currentState = GameState::MENU;
        targetZoom = 1.0f; // Сброс зума для удобства в меню
        return;
    }

    // Зум
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        targetZoom += wheel * 0.1f;
        targetZoom = Clamp(targetZoom, 0.1f, 5.0f);
    }
    camera.zoom = Lerp(camera.zoom, targetZoom, zoomSpeed * deltaTime);

    // КАМЕРА ВСЕГДА СЛЕДИТ ЗА ИГРОКОМ
    camera.target = player.getPosition();

    // Спавн
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer >= enemySpawnInterval) {
        spawnEnemy();
        enemySpawnTimer = 0;
    }

    // Логика
    player.update(deltaTime);
    player.shoot(bullets, camera);

    for (auto& bullet : bullets) bullet.update(deltaTime);
    for (auto& enemy : enemies) {
        enemy.update(deltaTime, player);
        enemy.attack(player);
    }

    checkCollision();
    cleanUp();

    if (!player.isAlive()) {
        previousState = GameState::PLAYING;
        currentState = GameState::MENU;
        // Рестарт
        score = 0; wave = 1; enemySpawnInterval = 2.0f;
        player = Player(Vector2{WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f});
        enemies.clear();
        bullets.clear();
        camera.target = player.getPosition();
    }
}

void Game::drawGameplay() {
    BeginMode2D(camera);

    // Отрисовка мира
    DrawRectangleLines(0, 0, WORLD_WIDTH, WORLD_HEIGHT, COLOR_WHITE);
    // Рисуем сетку только в видимой области для оптимизации (упрощенно - всю)
    for (int x = 0; x <= WORLD_WIDTH; x += 100) DrawLine(x, 0, x, WORLD_HEIGHT, COLOR_DARKGRAY);
    for (int y = 0; y <= WORLD_HEIGHT; y += 100) DrawLine(0, y, WORLD_WIDTH, y, COLOR_DARKGRAY);

    player.draw();
    for (const auto& enemy : enemies) enemy.draw();
    for (const auto& bullet : bullets) bullet.draw();

    EndMode2D();

    // UI
    DrawRectangle(10, 10, 200, 20, COLOR_WHITE);
    float hpPercent = (float)player.getHealth() / player.getMaxHealth();
    DrawRectangle(10, 10, 200 * hpPercent, 20, COLOR_GREEN);

    DrawText(TextFormat("Score: %d", score), 10, 40, 20, COLOR_WHITE);
    DrawText(TextFormat("Wave: %d", wave), 10, 70, 20, COLOR_WHITE);
    DrawText(TextFormat("Enemies: %d/%d", (int)enemies.size(), MAX_ENEMIES), 10, 100, 20, COLOR_YELLOW);
    DrawText("ESC - Menu", 10, 130, 20, GRAY);

    // Кнопка настроек в игре
    Rectangle btnSettingsInGame = { (float)GetScreenWidth() - 160, 10, 150, 40 };
    bool hoverSettings = CheckCollisionPointRec(GetMousePosition(), btnSettingsInGame);

    DrawRectangleRec(btnSettingsInGame, Fade(BLACK, 0.5f));
    DrawRectangleLinesEx(btnSettingsInGame, 2, hoverSettings ? WHITE : BLUE);
    DrawText("SETTINGS", btnSettingsInGame.x + 35, btnSettingsInGame.y + 12, 20, hoverSettings ? WHITE : BLUE);

    if (hoverSettings && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        previousState = GameState::PLAYING;
        currentState = GameState::SETTINGS;
        settingsOpenedFromGame = true;
    }
}

void Game::checkCollision() {
    for (auto& bullet : bullets) {
        if (!bullet.getIsActive()) continue;
        for (auto& enemy : enemies) {
            if (!enemy.getIsActive()) continue;
            if (bullet.checkCollision(enemy)) {
                enemy.takeDamage(bullet.getDamage());
                bullet.setIsActive(false);
                if (!enemy.getIsActive()) score += 10;
                break;
            }
        }
    }
}

void Game::spawnEnemy() {
    if (enemies.size() >= MAX_ENEMIES) return;

    float x = (float)GetRandomValue(0, WORLD_WIDTH - 32);
    float y = (float)GetRandomValue(0, WORLD_HEIGHT - 32);
    Vector2 pos = {x, y};

    int hp = 20 + (wave * 5);
    int dmg = 5 + wave;
    float spd = 50.f + (wave * 5);

    enemies.emplace_back(pos, 32, 32, hp, dmg, spd, 250.0f, 600.0f, Enemy::Behavior::CHASE);

    if (score >= wave * 100) {
        wave++;
        enemySpawnInterval = fmaxf(0.5f, enemySpawnInterval - 0.1f);
    }
}

void Game::cleanUp() {
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.getIsActive() || b.isOffScreen(); }), bullets.end());
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return !e.getIsActive(); }), enemies.end());
}

// ================= МЕНЮ =================

void Game::updateMenu(float deltaTime) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    Rectangle btnPlay = { (float)sw/2 - 100, 200, 200, 50 };
    Rectangle btnSettings = { (float)sw/2 - 100, 270, 200, 50 };
    Rectangle btnExit = { (float)sw/2 - 100, 340, 200, 50 };

    if (CheckCollisionPointRec(GetMousePosition(), btnPlay)) hoveredButton = 0;
    else if (CheckCollisionPointRec(GetMousePosition(), btnSettings)) hoveredButton = 1;
    else if (CheckCollisionPointRec(GetMousePosition(), btnExit)) hoveredButton = 2;
    else hoveredButton = -1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (hoveredButton == 0) {
            score = 0; wave = 1; enemySpawnInterval = 2.0f;
            player = Player(Vector2{WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f});
            enemies.clear(); bullets.clear();
            camera.target = player.getPosition(); // Фокус на игрока
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
    DrawText("HappySquare", sw/2 - MeasureText("HappySquare", 60)/2, 80, 60, BLUE);

    Rectangle btnPlay = { (float)sw/2 - 100, 200, 200, 50 };
    Rectangle btnSettings = { (float)sw/2 - 100, 270, 200, 50 };
    Rectangle btnExit = { (float)sw/2 - 100, 340, 200, 50 };

    drawNeonButton(btnPlay, "PLAY", hoveredButton == 0, BLUE);
    drawNeonButton(btnSettings, "SETTINGS", hoveredButton == 1, BLUE);
    drawNeonButton(btnExit, "EXIT", hoveredButton == 2, RED);
}

// ================= НАСТРОЙКИ =================

void Game::updateSettings(float deltaTime) {
    int sw = GetScreenWidth();

    // Кнопка BACK
    Rectangle btnBack = { (float)sw/2 - 100, 600, 200, 50 };
    bool hoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);

    if (hoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        applyResolution(currentResIndex, currentMonitorIndex);
        currentState = previousState;
        return;
    }

    // Выбор разрешения
    Rectangle rectRes = { (float)sw/2 - 150, 150, 300, 40 };
    if (CheckCollisionPointRec(GetMousePosition(), rectRes) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        currentResIndex = (currentResIndex + 1) % resolutions.size();
        applyResolution(currentResIndex, currentMonitorIndex); // Предпросмотр
    }

    // Выбор монитора (если их > 1)
    int monitorCount = GetMonitorCount();
    if (monitorCount > 1) {
        Rectangle rectMon = { (float)sw/2 - 150, 210, 300, 40 };
        if (CheckCollisionPointRec(GetMousePosition(), rectMon) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentMonitorIndex = (currentMonitorIndex + 1) % monitorCount;
            applyResolution(currentResIndex, currentMonitorIndex);
        }
    }

    // Слайдеры
    Rectangle rectMusic = { (float)sw/2 - 100, 300, 200, 20 };
    if (CheckCollisionPointRec(GetMousePosition(), rectMusic) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        musicVolume = Clamp((GetMousePosition().x - rectMusic.x) / rectMusic.width, 0.0f, 1.0f);
    }

    Rectangle rectSfx = { (float)sw/2 - 100, 360, 200, 20 };
    if (CheckCollisionPointRec(GetMousePosition(), rectSfx) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        sfxVolume = Clamp((GetMousePosition().x - rectSfx.x) / rectSfx.width, 0.0f, 1.0f);
    }
}

void Game::drawSettings() {
    int sw = GetScreenWidth();
    DrawText("SETTINGS", sw/2 - MeasureText("SETTINGS", 40)/2, 50, 40, BLUE);

    // Разрешение
    DrawText("Resolution:", sw/2 - 150, 120, 20, WHITE);
    const char* resName = resolutions[currentResIndex].name;
    Rectangle rectRes = { (float)sw/2 - 150, 150, 300, 40 };
    bool hoverRes = CheckCollisionPointRec(GetMousePosition(), rectRes);
    DrawRectangleLinesEx(rectRes, 2, hoverRes ? WHITE : BLUE);
    DrawText(resName, rectRes.x + (rectRes.width - MeasureText(resName, 20))/2, rectRes.y + 10, 20, hoverRes ? WHITE : BLUE);
    if(hoverRes) DrawText("< Click >", rectRes.x + 120, rectRes.y + 35, 10, GRAY);

    // Монитор (если > 1)
    int monitorCount = GetMonitorCount();
    if (monitorCount > 1) {
        DrawText("Monitor:", sw/2 - 150, 180, 20, WHITE);
        const char* monName = TextFormat("Monitor %d", currentMonitorIndex + 1);
        Rectangle rectMon = { (float)sw/2 - 150, 210, 300, 40 };
        bool hoverMon = CheckCollisionPointRec(GetMousePosition(), rectMon);
        DrawRectangleLinesEx(rectMon, 2, hoverMon ? WHITE : BLUE);
        DrawText(monName, rectMon.x + (rectMon.width - MeasureText(monName, 20))/2, rectMon.y + 10, 20, hoverMon ? WHITE : BLUE);
        if(hoverMon) DrawText("< Click >", rectMon.x + 120, rectMon.y + 35, 10, GRAY);
    }

    drawSlider({(float)sw/2 - 100, 300, 200, 20}, musicVolume, "Music Volume");
    drawSlider({(float)sw/2 - 100, 360, 200, 20}, sfxVolume, "SFX Volume");

    Rectangle btnBack = { (float)sw/2 - 100, 600, 200, 50 };
    bool hoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    drawNeonButton(btnBack, "BACK", hoverBack, BLUE);

    DrawText(settingsOpenedFromGame ? "Paused" : "Main Menu", sw/2 - 40, 580, 15, GRAY);
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
