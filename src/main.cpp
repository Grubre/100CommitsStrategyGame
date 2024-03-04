#include <iostream>
#include <entt.hpp>
#include "raylib.h"

auto main() -> int {
    const int screen_width = 800;
    const int screen_height = 450;
    const int fps = 60;

    entt::registry registry;


    auto entity = registry.create();
    registry.emplace<std::string>(entity, "Hello, World!");

    InitWindow(screen_width, screen_height, "Hello World!");

    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(registry.get<std::string>(entity).c_str(), 190, 200, 20, BLACK);
        EndDrawing();
    }

    return 0;
}
