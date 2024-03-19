#include "camera.hpp"
#include "drawing.hpp"
#include "raylib.h"
#include "raymath.h"
#include "systems.hpp"
#include <entt.hpp>
#include <iostream>

#include "common_components.hpp"
#include "terrain.hpp"

void handle_input(entt::registry &registry) { stratgame::handle_camera_input(registry); }

void setup_raylib() {
    static constexpr int screen_width = 1920;
    static constexpr int screen_height = 1080;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(screen_width, screen_height, "Hello World!");
}

auto setup_entt() -> entt::registry {
    entt::registry registry;

    // NOTE: Movement depends on Transform
    // NOTE: Resets Transform when Movement is added
    registry.on_construct<stratgame::Movement>().connect<&entt::registry::emplace_or_replace<stratgame::Transform>>();

    return registry;
}

auto main() -> int {
    setup_raylib();

    auto registry = setup_entt();

    auto terrain_entity = registry.create();
    auto terrain = stratgame::generate_terrain_model(50, 50);
    auto terrain_shader = stratgame::generate_terrain_shader("../resources/shaders/terrain.vert",
                                                             "../resources/shaders/terrain.frag", 5.0f);
    registry.emplace<stratgame::ModelComponent>(terrain_entity, terrain, Vector3{0, 0, 0});
    registry.emplace<stratgame::ShaderComponent>(terrain_entity, terrain_shader);
    registry.emplace<stratgame::DrawModelWireframeComponent>(terrain_entity);

    const auto camera_entity = stratgame::create_camera(registry);

    while (!WindowShouldClose()) {
        handle_input(registry);

        stratgame::update_transform(registry);

        stratgame::update_camera(registry);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        const auto &camera = registry.get<stratgame::Camera>(camera_entity);
        BeginMode3D(camera.camera3d);

        stratgame::draw_models(registry);

        stratgame::draw_model_wireframes(registry);

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
