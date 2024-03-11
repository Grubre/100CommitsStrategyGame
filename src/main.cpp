#include "camera.hpp"
#include "raylib.h"
#include "raymath.h"
#include <entt.hpp>

#include "common_components.hpp"

void update_movement(entt::registry &registry) {
    auto view = registry.view<stratgame::Movement>();
    for (auto entity : view) {
        auto &movement = view.get<stratgame::Movement>(entity);
        movement.velocity = Vector3Normalize(movement.velocity);
        movement.velocity = Vector3Scale(movement.velocity, movement.speed);
    }
}

void handle_input(entt::registry &registry) { stratgame::handle_camera_input(registry); }

auto main() -> int {
    const int screen_width = 800;
    const int screen_height = 450;
    const int fps = 60;

    entt::registry registry;

    Vector3 cubePosition = {0.0f, 0.0f, 0.0f};

    const auto camera_entity = stratgame::create_camera(registry);

    InitWindow(screen_width, screen_height, "Hello World!");
    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        handle_input(registry);

        update_movement(registry);

        stratgame::update_camera(registry);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        const auto &camera = registry.get<stratgame::Camera>(camera_entity);
        BeginMode3D(camera.camera3d);

        DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
        DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

        DrawGrid(100, 1.0f);

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    return 0;
}
