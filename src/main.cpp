#include "camera.hpp"
#include "drawing.hpp"
#include "raylib.h"
#include "raymath.h"
#include "systems.hpp"
#include <entt.hpp>

#include "common_components.hpp"
#include "terrain.hpp"

void handle_input(entt::registry &registry) { stratgame::handle_camera_input(registry); }

auto main() -> int {
    const int screen_width = 1920;
    const int screen_height = 1080;
    const int fps = 60;

    entt::registry registry;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(screen_width, screen_height, "Hello World!");

    const auto camera_entity = stratgame::create_camera(registry);
    auto terrain = stratgame::generate_terrain_model(50, 50);

    auto terrain_shader = stratgame::generate_terrain_shader("../resources/shaders/terrain.vert",
                                                             "../resources/shaders/terrain.frag", 5.0f);

    auto terrain_entity = registry.create();
    registry.emplace<stratgame::ModelComponent>(terrain_entity, terrain, Vector3{0, 0, 0});
    registry.emplace<stratgame::ShaderComponent>(terrain_entity, terrain_shader);
    registry.emplace<stratgame::DrawModelWireframeComponent>(terrain_entity);

    // Movement depends on Transform
    registry.on_construct<stratgame::Movement>().connect<&entt::registry::emplace_or_replace<stratgame::Transform>>();

    while (!WindowShouldClose()) {
        handle_input(registry);

        stratgame::update_movement(registry);

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
