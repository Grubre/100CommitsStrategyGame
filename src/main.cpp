#include "raylib.h"
#include "raymath.h"
#include <entt.hpp>
#include <iostream>

using Velocity = Vector3;

void update_camera_position(entt::registry &registry) {
    auto view = registry.view<Camera3D, Velocity>();
    for (auto entity : view) {
        auto &velocity = view.get<Velocity>(entity);
        auto &camera = view.get<Camera3D>(entity);
        // print the camera velocity
        std::cout << "Camera velocity: " << velocity.x << ", " << velocity.y << ", " << velocity.z << std::endl;
        camera.position = Vector3Add(camera.position, {velocity.x, velocity.y, 0});
    }
}

auto main() -> int {
    const int screen_width = 800;
    const int screen_height = 450;
    const int fps = 60;

    entt::registry registry;

    Vector3 cubePosition = {0.0f, 0.0f, 0.0f};

    auto camera_entity = registry.create();
    registry.emplace<Camera3D>(camera_entity, Camera3D{.position = Vector3{0.0f, 10.0f, 10.0f},
                                                       .target = Vector3{0.0f, 0.0f, 0.0f},
                                                       .up = Vector3{0.0f, 1.0f, 0.0f},
                                                       .fovy = 45.0f,
                                                       .projection = CAMERA_PERSPECTIVE});

    InitWindow(screen_width, screen_height, "Hello World!");

    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_W)) {
            registry.emplace_or_replace<Velocity>(camera_entity, (Vector3){0, 0.1f, 0});
        } else if (IsKeyDown(KEY_S)) {
            registry.emplace_or_replace<Velocity>(camera_entity, (Vector3){0, -0.1f, 0});
        } else {
            registry.emplace_or_replace<Velocity>(camera_entity, (Vector3){0, -0, 0});
        }

        update_camera_position(registry);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        const auto &camera = registry.get<Camera3D>(camera_entity);
        BeginMode3D(camera);

        DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
        DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

        DrawGrid(10, 1.0f);

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    return 0;
}
