#include "camera.hpp"
#include "drawing.hpp"
#include "minion.hpp"
#include "raylib.h"
#include "systems.hpp"
#include "tasks.hpp"
#include <entt.hpp>

#include "common_components.hpp"
#include "terrain.hpp"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

auto main() -> int {
    stratgame::setup_raylib();

    auto registry = stratgame::setup_entt();

    auto terrain_entity = registry.create();
    auto [terrain, heights] = stratgame::generate_terrain_model(200, 200);
    auto terrain_shader = stratgame::generate_terrain_shader("../resources/shaders/terrain.vs",
                                                             "../resources/shaders/terrain.fs", 5.0f);
    registry.emplace<stratgame::ModelComponent>(terrain_entity, terrain);
    registry.emplace<stratgame::ShaderComponent>(terrain_entity, terrain_shader);
    registry.emplace<stratgame::Transform>(terrain_entity, Vector3{0, 0, 0});
    registry.emplace<stratgame::DrawModelWireframeComponent>(terrain_entity);
    registry.emplace<stratgame::GeneratedTerrain::Heights>(terrain_entity, heights);

    registry.emplace<stratgame::TerrainClick>(terrain_entity);

    const auto tree_model = LoadModel("../resources/tree/tree.gltf");
    auto tree_instancing_shader =
        LoadShader("../resources/shaders/instancing.vs", "../resources/shaders/instancing.fs");
    tree_instancing_shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(tree_instancing_shader, "mvp");
    tree_instancing_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(tree_instancing_shader, "viewPos");
    tree_instancing_shader.locs[SHADER_LOC_MATRIX_MODEL] =
        GetShaderLocationAttrib(tree_instancing_shader, "instanceTransform");

    for (auto i = 0; i < tree_model.materialCount; i++) {
        tree_model.materials[i].shader = tree_instancing_shader;
        tree_model.materials[i].maps[MATERIAL_MAP_ALBEDO].texture = LoadTexture("../resources/tree/treeDiffuse.png");
    }
    auto tree_model_entity = stratgame::register_instanceable_model(registry, tree_model);

    // for (int i = 0; i < 50; i++) {
    //     for (int j = 0; j < 50; j++) {
    //         const auto height = heights[4 * i + 4 * j * 200];
    //         const auto x_offset = ((float)rand() / RAND_MAX) * 2 - 1;
    //         const auto z_offset = ((float)rand() / RAND_MAX) * 2 - 1;
    //         stratgame::create_model_instance(registry, tree_model_entity,
    //                                          Vector3{4 * (float)i + x_offset, height, 4 * (float)j + z_offset},
    //                                          registry.create());
    //     }
    // }

    auto selected_entity = registry.create();
    registry.emplace<stratgame::SelectedState>(selected_entity);

    const auto camera_entity = stratgame::create_camera(registry);

    stratgame::register_team(registry, RED);
    stratgame::register_team(registry, BLUE);

    // for (auto i = 0; i < 10; i++) {
    //     stratgame::create_minion(registry, {static_cast<float>(i * 2), static_cast<float>(i * 2)}, rand() % 2);
    // }

    bool toggle_wireframe = false;
    GuiLoadStyleDefault();

    while (!WindowShouldClose()) {
        auto &camera = registry.get<stratgame::Camera>(camera_entity);
        // ======================================
        // UPDATE SYSTEMS
        // ======================================
        stratgame::handle_input(registry);
        stratgame::update_transform(registry);
        stratgame::update_camera(registry);
        stratgame::update_minion_heights(registry);
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

    UnloadModel(terrain);
    CloseWindow();

    return 0;
}
