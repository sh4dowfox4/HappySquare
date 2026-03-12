#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

// Размеры экрана
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int TARGET_FPS = 180;

// Размеры игрового мира
constexpr int WORLD_WIDTH = 5000;
constexpr int WORLD_HEIGHT = 5000;

// Цвета (алиасы для удобства)
constexpr Color COLOR_GREEN = GREEN;
constexpr Color COLOR_MAROON = MAROON;
constexpr Color COLOR_YELLOW = YELLOW;
constexpr Color COLOR_WHITE = WHITE;
constexpr Color COLOR_BLACK = BLACK;
constexpr Color COLOR_DARKGRAY = DARKGRAY;

#endif // COMMON_H
