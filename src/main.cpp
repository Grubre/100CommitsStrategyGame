#include "assets_loader.hpp"
#include "camera.hpp"
#include "drawing.hpp"
#include "homeless_functions.hpp"
#include "imgui.h"
#include "minion.hpp"
#include "raylib.h"
#include "rlImGui.h"
#include "systems.hpp"
#include "tasks.hpp"
#include <entt.hpp>
#include <print>

#include "common_components.hpp"
#include "terrain.hpp"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

auto main() -> int {
    stratgame::setup_raylib();

    auto registry = stratgame::setup_entt();
    const auto world_entity = registry.create();

    auto terrain_shader = stratgame::generate_terrain_shader(
        stratgame::load_asset(LoadShader, "shaders/terrain.vs", "shaders/terrain.fs"), 5.0f);
    auto noise = SimplexNoise();
    const auto terrain_generator = stratgame::generate_terrain(registry, 32 * 16, 2, noise, terrain_shader);

    registry.emplace<stratgame::TerrainClick>(world_entity);

    auto selected_entity = registry.create();
    registry.emplace<stratgame::SelectedState>(selected_entity);

    const auto camera_entity = stratgame::create_camera(registry);

    stratgame::register_team(registry, RED);
    stratgame::register_team(registry, BLUE);

    for (auto i = 0; i < 10; i++) {
        stratgame::create_minion(registry, {static_cast<float>(i * 2), static_cast<float>(i * 2)}, rand() % 2);
    }

    bool toggle_wireframe = false;
    GuiLoadStyleDefault();

    rlImGuiSetup(true);

    while (!WindowShouldClose()) {
        auto &camera = registry.get<stratgame::Camera>(camera_entity);
        // ======================================
        // UPDATE SYSTEMS
        // ======================================
        stratgame::flag_culled_models(registry);
        stratgame::handle_input(registry);
        stratgame::update_transform(registry);
        stratgame::update_camera(registry);
        // stratgame::update_minion_heights(registry);
        stratgame::update_tasks(registry);

        // ======================================

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera.camera3d);

        // ======================================
        // DRAW SYSTEMS
        // ======================================
        stratgame::draw_models(registry);
        if (toggle_wireframe) {
            stratgame::draw_model_wireframes(registry);
        }
        stratgame::draw_models_instanced(registry);
        // ======================================

        DrawLine3D({-1000, 0, 0}, {1000, 0, 0}, RED);
        DrawLine3D({0, 0, -1000}, {0, 0, 1000}, BLUE);

        EndMode3D();

        // ======================================
        // DRAW GUI
        // ======================================
        GuiCheckBox(Rectangle{50, 50, 30, 30}, "Toggle wireframe", &toggle_wireframe);
        GuiSliderBar(Rectangle{50, 100, 100, 20}, nullptr, "Camera speed", &camera.speed, 0.f, 500.f);

        DrawFPS(10, 10);

        EndDrawing();
    }
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
