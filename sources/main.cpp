#include "game.h"

bool g_debugMode = false;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "HappySquare 0.0.4");
    SetTargetFPS(TARGET_FPS);

    Game game;
    game.run();

    CloseWindow();
    return 0;
}
