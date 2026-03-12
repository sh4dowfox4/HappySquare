#include "game.h"

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "42 - Big Map Refactored");
    SetTargetFPS(TARGET_FPS);
    
    Game game;
    game.run();
    
    CloseWindow();
    return 0;
}
