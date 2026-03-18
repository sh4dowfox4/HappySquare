#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 800;
constexpr int TARGET_FPS = 60;

// Размеры игрового мира
constexpr int WORLD_WIDTH = 7000;
constexpr int WORLD_HEIGHT = 7000;

// Цвета (алиасы для удобства)
constexpr Color COLOR_GREEN = GREEN;
constexpr Color COLOR_MAROON = MAROON;
constexpr Color COLOR_YELLOW = YELLOW;
constexpr Color COLOR_WHITE = WHITE;
constexpr Color COLOR_BLACK = BLACK;
constexpr Color COLOR_DARKGRAY = DARKGRAY;

#endif // COMMON_H
