#include "SimplexNoise.h"
#include "camera.hpp"
#include "raylib.h"
#include "raymath.h"
#include <entt.hpp>
#include <iostream>
#include <span>

#include "common_components.hpp"
#include "terrain.hpp"

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
    const int screen_width = 1920;
    const int screen_height = 1080;
    const int fps = 60;

    entt::registry registry;

    const auto camera_entity = stratgame::create_camera(registry);

    InitWindow(screen_width, screen_height, "Hello World!");
    SetTargetFPS(fps);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    auto terrain = stratgame::generate_terrain_model(100, 100);

    auto terrain_shader = stratgame::generate_terrain_shader("../resources/shaders/terrain.vert",
                                                             "../resources/shaders/terrain.frag", 5.0f);

    while (!WindowShouldClose()) {
        handle_input(registry);

        update_movement(registry);

        stratgame::update_camera(registry);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        const auto &camera = registry.get<stratgame::Camera>(camera_entity);
        BeginMode3D(camera.camera3d);

        auto shader_backup = terrain.materials[0].shader;
        terrain.materials[0].shader = terrain_shader;
        DrawModel(terrain, {0, 0, 0}, 1.0f, WHITE);
        terrain.materials[0].shader = shader_backup;

        DrawModelWires(terrain, {0, 0, 0}, 1.0f, Fade(LIGHTGRAY, 0.6f));

        DrawLine3D({-1000, 0, 0}, {1000, 0, 0}, RED);
        DrawLine3D({0, 0, -1000}, {0, 0, 1000}, BLUE);

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadModel(terrain);
    CloseWindow();

    return 0;
}
