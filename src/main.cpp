#include "camera.hpp"
#include "drawing.hpp"
#include "minion.hpp"
#include "raylib.h"
#include "systems.hpp"
#include "tasks.hpp"
#include <entt.hpp>
#include <optional>

#include "common_components.hpp"
#include "terrain.hpp"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void handle_input(entt::registry &registry) { stratgame::handle_camera_input(registry); }

void setup_raylib() {
    static constexpr int screen_width = 2560;
    static constexpr int screen_height = 1440;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(screen_width, screen_height, "Hello World!");
}

using team_color_map = std::unordered_map<int, Color>;
void add_team(entt::registry &registry, const Color &color) {
    static int team_id = 0;

    if (team_id == 0) {
        const auto team_color_map_entity = registry.create();
        registry.emplace<team_color_map>(team_color_map_entity);
    }

    auto &team_colors = registry.get<team_color_map>(*registry.view<team_color_map>().begin());
    team_colors[team_id] = color;

    team_id++;
}

auto setup_entt() -> entt::registry {
    entt::registry registry;

    // NOTE: Movement depends on Transform
    // NOTE: Resets Transform when Movement is added
    registry.on_construct<stratgame::Movement>().connect<&entt::registry::emplace_or_replace<stratgame::Transform>>();

    // NOTE: Minions must have Transform, BaseStats and ModelComponent
    registry.on_construct<stratgame::Minion>().connect<[](entt::registry &registry, entt::entity entity) {
        registry.emplace<stratgame::Transform>(entity);
        registry.emplace<stratgame::BaseStats>(entity);
        registry.emplace<stratgame::Selectable>(entity);

        const auto &minion = registry.get<stratgame::Minion>(entity);
        auto model = LoadModelFromMesh(GenMeshSphere(1.f, 16, 16));

        const auto team_color_map_entity = registry.view<team_color_map>().begin()[0];
        const auto &team_colors = registry.get<team_color_map>(team_color_map_entity);
        const auto color = team_colors.at(minion.team_id);

        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = color;
        registry.emplace<stratgame::ModelComponent>(entity, model);
    }>();

    return registry;
}

struct TerrainClick {
    std::optional<Vector2> position;
};

struct SelectedState {
    int32_t selected_entities_count = 0u;
};

void handle_clicks(entt::registry &registry) {
    const auto terrain_entity = registry.view<TerrainClick>().begin()[0];
    const auto terrain = registry.get<stratgame::ModelComponent>(terrain_entity);
    const auto camera_entity = registry.view<stratgame::Camera>().begin()[0];
    const auto &camera = registry.get<stratgame::Camera>(camera_entity);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        const auto mouse_pos = GetMousePosition();
        const auto ray = GetMouseRay(mouse_pos, camera.camera3d);
        // check hit between terrain and ray
        const auto hit = GetRayCollisionMesh(ray, terrain.model.meshes[0], terrain.model.transform);

        registry.patch<TerrainClick>(terrain_entity, [&](TerrainClick &click) {
            click.position = hit.hit ? std::optional{Vector2{hit.point.x, hit.point.z}} : std::nullopt;
        });

        auto minions = registry.view<stratgame::Minion, stratgame::ModelComponent, stratgame::Transform>();
        for (auto minion : minions) {
            const auto &transform = registry.get<stratgame::Transform>(minion);

            const auto minion_hit = GetRayCollisionSphere(ray, transform.position, 1.f);

            if (minion_hit.hit) {
                registry.patch<stratgame::Selectable>(minion, [&](stratgame::Selectable &selectable) {
                    selectable.selected = !selectable.selected;

                    registry.patch<SelectedState>(registry.view<SelectedState>().begin()[0],
                                                  [&](SelectedState &selected) {
                                                      selected.selected_entities_count += selectable.selected ? 1 : -1;
                                                  });
                });
            }
        }
    }
}

auto main() -> int {
    setup_raylib();

    auto registry = setup_entt();

    auto terrain_entity = registry.create();
    auto [terrain, heights] = stratgame::generate_terrain_model(200, 200);
    auto terrain_shader = stratgame::generate_terrain_shader("../resources/shaders/terrain.vert",
                                                             "../resources/shaders/terrain.frag", 5.0f);
    registry.emplace<stratgame::ModelComponent>(terrain_entity, terrain);
    registry.emplace<stratgame::ShaderComponent>(terrain_entity, terrain_shader);
    registry.emplace<stratgame::Transform>(terrain_entity, Vector3{0, 0, 0});
    registry.emplace<stratgame::DrawModelWireframeComponent>(terrain_entity);
    registry.emplace<stratgame::GeneratedTerrain::Heights>(terrain_entity, heights);

    registry.emplace<TerrainClick>(terrain_entity);

    const auto tree_model = LoadModel("../resources/tree/tree.gltf");
    auto tree_instancing_shader = LoadShader("../resources/shaders/instancing.vs", "../resources/shaders/instancing.fs");
    tree_instancing_shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(tree_instancing_shader, "mvp");
    tree_instancing_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(tree_instancing_shader, "viewPos");
    tree_instancing_shader.locs[SHADER_LOC_MATRIX_MODEL] =
        GetShaderLocationAttrib(tree_instancing_shader, "instanceTransform");

    for (auto i = 0; i < tree_model.materialCount; i++) {
        tree_model.materials[i].shader = tree_instancing_shader;
        tree_model.materials[i].maps[MATERIAL_MAP_ALBEDO].texture = LoadTexture("../resources/tree/treeDiffuse.png");
    }
    auto tree_model_entity = stratgame::register_instanceable_model(registry, tree_model);

    for(int i = 0; i < 50; i++) {
        for(int j = 0; j < 50; j++) {
            const auto height = heights[4*i + 4*j * 200];
            const auto x_offset = ((float)rand() / RAND_MAX) * 2 - 1;
            const auto z_offset = ((float)rand() / RAND_MAX) * 2 - 1;
            stratgame::create_model_instance(registry, tree_model_entity, Vector3{4*(float)i + x_offset, height, 4*(float)j + z_offset}, registry.create());
        }
    }

    auto selected_entity = registry.create();
    registry.emplace<SelectedState>(selected_entity);

    const auto camera_entity = stratgame::create_camera(registry);

    add_team(registry, RED);
    add_team(registry, BLUE);

    for (auto i = 0; i < 10; i++) {
        stratgame::create_minion(registry, {static_cast<float>(i * 2), static_cast<float>(i * 2)}, rand() % 2);
    }

    bool toggle_wireframe = false;
    GuiLoadStyleDefault();

    while (!WindowShouldClose()) {
        const auto &camera = registry.get<stratgame::Camera>(camera_entity);
        // ======================================
        // UPDATE SYSTEMS
        // ======================================
        handle_clicks(registry);

        const auto terrain_click = registry.get<TerrainClick>(terrain_entity);
        if (terrain_click.position) {
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                auto selected_minions = registry.view<stratgame::Minion, stratgame::Selectable>();

                for (auto minion : selected_minions) {
                    const auto selected = registry.get<stratgame::Selectable>(minion).selected;

                    if (!selected) {
                        continue;
                    }

                    add_task(registry, minion, stratgame::WalkToTask{*terrain_click.position, 5.f});
                }
            }
        }

        handle_input(registry);
        stratgame::update_transform(registry);
        stratgame::update_camera(registry);
        stratgame::update_minion_heights(registry);
        stratgame::update_tasks(registry);

        // ======================================

        BeginDrawing();
        GuiCheckBox(Rectangle{50, 100, 30, 30}, "Toggle wireframe", &toggle_wireframe);

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

        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadModel(terrain);
    CloseWindow();

    return 0;
}
