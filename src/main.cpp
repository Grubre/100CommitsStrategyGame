#include "camera.hpp"
#include "drawing.hpp"
#include "raylib.h"
#include "raymath.h"
#include "systems.hpp"
#include <entt.hpp>
#include <iostream>
#include <optional>

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

using team_color_map = std::unordered_map<int, Color>;
void add_team(entt::registry &registry, const Color& color) {
    static int team_id = 0;

    if (team_id == 0) {
        const auto team_color_map_entity = registry.create();
        registry.emplace<team_color_map>(team_color_map_entity);
    }

    auto &team_colors = registry.get<team_color_map>(*registry.view<team_color_map>().begin());
    team_colors[team_id] = color;

    team_id++;
}

struct Minion {
    int team_id;
};

struct BaseStats {
    int health{100};
    int attack{10};
};

struct Selectable {
    bool selected{false};
};

auto setup_entt() -> entt::registry {
    entt::registry registry;

    // NOTE: Movement depends on Transform
    // NOTE: Resets Transform when Movement is added
    registry.on_construct<stratgame::Movement>().connect<&entt::registry::emplace_or_replace<stratgame::Transform>>();

    // NOTE: Minions must have Transform, BaseStats and ModelComponent
    registry.on_construct<Minion>().connect<[](entt::registry &registry, entt::entity entity) {
        registry.emplace<stratgame::Transform>(entity);
        registry.emplace<BaseStats>(entity);
        registry.emplace<Selectable>(entity);

        const auto &minion = registry.get<Minion>(entity);
        auto model = LoadModelFromMesh(GenMeshSphere(1.f, 16, 16));

        const auto team_color_map_entity = registry.view<team_color_map>().begin()[0];
        const auto &team_colors = registry.get<team_color_map>(team_color_map_entity);
        const auto color = team_colors.at(minion.team_id);

        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = color;
        registry.emplace<stratgame::ModelComponent>(entity, model);
    }>();

    return registry;
}

auto create_minion(entt::registry &registry, Vector2 position, int team_id) -> entt::entity {
    auto entity = registry.create();
    registry.emplace<Minion>(entity, team_id);

    auto &transform = registry.get<stratgame::Transform>(entity);
    transform.position.x = position.x;
    transform.position.z = position.y;
    return entity;
}

void update_minion_heights(entt::registry &registry) {
    const auto height_entity = registry.view<const stratgame::GeneratedTerrain::Heights>();
    const auto heights = registry.get<stratgame::GeneratedTerrain::Heights>(*height_entity.begin());

    auto minions = registry.view<Minion>();

    for (auto minion : minions) {
        auto &transform = registry.get<stratgame::Transform>(minion);

        const auto x = static_cast<int>(transform.position.x);
        const auto z = static_cast<int>(transform.position.z);

        // NOTE: The +1 is because minions are temporarily spheres of radius 1
        transform.position.y = heights[z * 200 + x] + 1.f;
    }
}

struct TerrainClick {
    std::optional<Vector2> position;
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

        auto minions = registry.view<Minion, stratgame::ModelComponent, stratgame::Transform>();
        for (auto minion : minions) {
            const auto &transform = registry.get<stratgame::Transform>(minion);

            const auto minion_hit = GetRayCollisionSphere(ray, transform.position, 1.f);

            if (minion_hit.hit) {
                std::cout << "Minion hit!" << std::endl;
            }

            registry.patch<Selectable>(minion, [&](Selectable &selectable) { selectable.selected = minion_hit.hit; });
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

    const auto camera_entity = stratgame::create_camera(registry);

    add_team(registry, RED);
    add_team(registry, BLUE);

    for (auto i = 0; i < 10; i++) {
        create_minion(registry, {static_cast<float>(i * 2), static_cast<float>(i * 2)}, rand() % 2);
    }

    while (!WindowShouldClose()) {
        const auto &camera = registry.get<stratgame::Camera>(camera_entity);
        // ======================================
        // UPDATE SYSTEMS
        // ======================================
        handle_clicks(registry);

        const auto terrain_click = registry.get<TerrainClick>(terrain_entity);
        if (terrain_click.position) {
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                create_minion(registry, *terrain_click.position, rand() % 2);
            }
        }

        handle_input(registry);
        stratgame::update_transform(registry);
        stratgame::update_camera(registry);
        update_minion_heights(registry);

        // ======================================

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera.camera3d);

        // ======================================
        // DRAW SYSTEMS
        // ======================================
        stratgame::draw_models(registry);
        stratgame::draw_model_wireframes(registry);
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
