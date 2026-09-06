cmake_minimum_required(VERSION 3.20)
project(ShortcutBar C)

set(CMAKE_C_STANDARD 11)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

add_executable(ShortcutBar WIN32 main.c)

target_link_libraries(ShortcutBar PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_ttf::SDL2_ttf user32 shell32)

