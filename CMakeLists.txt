cmake_minimum_required(VERSION 3.10)
project(ArenaSurvivor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ищем установленный raylib
find_package(raylib REQUIRED)

add_executable(game main.cpp)

# Линкуем библиотеку
target_link_libraries(game raylib)

# Если на Линуксе нужны доп либы (поток, математика)
if(UNIX AND NOT APPLE)
    target_link_libraries(game m pthread)
endif()
