#include "raylib.h"
#include "raymath.h"
#include <entt.hpp>

#include "common_components.hpp"

void handle_camera_input(entt::registry &registry) {
    auto view = registry.view<Camera3D, Movement>();
    for (auto entity : view) {
        auto &movement = view.get<Movement>(entity);
        if (IsKeyDown(KEY_W)) {
            movement.velocity.x -= 1.f;
            movement.velocity.z -= 1.f;
        }
        if (IsKeyDown(KEY_S)) {
            movement.velocity.x += 1.f;
            movement.velocity.z += 1.f;
        }
        if (IsKeyDown(KEY_A)) {
            movement.velocity.x -= 1.f;
            movement.velocity.z += 1.f;
        }
        if (IsKeyDown(KEY_D)) {
            movement.velocity.x += 1.f;
            movement.velocity.z -= 1.f;
        }
        const auto scroll = GetMouseWheelMove() * 1.5f;
        movement.velocity.x -= scroll;
        movement.velocity.y -= scroll;
        movement.velocity.z -= scroll;
    }
}

void update_movement(entt::registry &registry) {
    auto view = registry.view<Movement>();
    for (auto entity : view) {
        auto &movement = view.get<Movement>(entity);
        movement.velocity = Vector3Normalize(movement.velocity);
        movement.velocity = Vector3Scale(movement.velocity, movement.speed);
    }
}

void handle_input(entt::registry &registry) { handle_camera_input(registry); }

void update_camera_position(entt::registry &registry) {
    auto view = registry.view<Camera3D, Movement>();
    for (auto entity : view) {
        auto &movement = view.get<Movement>(entity);
        auto &camera = view.get<Camera3D>(entity);
        camera.position = Vector3Add(camera.position, {movement.velocity.x, movement.velocity.y, movement.velocity.z});
        camera.target = Vector3Add(camera.target, {movement.velocity.x, movement.velocity.y, movement.velocity.z});
        movement.velocity = {0, 0, 0};
    }
}

auto main() -> int {
    const int screen_width = 800;
    const int screen_height = 450;
    const int fps = 60;

    entt::registry registry;

    Vector3 cubePosition = {0.0f, 0.0f, 0.0f};

    auto camera_entity = create_camera(registry);

    InitWindow(screen_width, screen_height, "Hello World!");
    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        handle_input(registry);

        update_movement(registry);

        update_camera_position(registry);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        const auto &camera = registry.get<Camera3D>(camera_entity);
        BeginMode3D(camera);

        DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
        DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

        DrawGrid(100, 1.0f);

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    return 0;
}
