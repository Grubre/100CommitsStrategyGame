#include "camera.hpp"
#include "drawing.hpp"
#include "raylib.h"
#include "systems.hpp"
#include <entt.hpp>

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

struct Minion {
    Color color{RED};
};

struct BaseStats {
    int health{100};
    int attack{10};
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

        const auto &minion = registry.get<Minion>(entity);
        auto model = LoadModelFromMesh(GenMeshSphere(1.f, 16, 16));
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = minion.color;
        registry.emplace<stratgame::ModelComponent>(entity, model);
    }>();

    return registry;
}

auto create_minion(entt::registry &registry, Vector2 position) -> entt::entity {
    auto entity = registry.create();
    registry.emplace<Minion>(entity);

    auto &transform = registry.get<stratgame::Transform>(entity);
    transform.position.x = position.x;
    transform.position.z = position.y;
    return entity;
}

auto main() -> int {
    setup_raylib();

    auto registry = setup_entt();

    auto terrain_entity = registry.create();
    auto terrain = stratgame::generate_terrain_model(50, 50);
    auto terrain_shader = stratgame::generate_terrain_shader("../resources/shaders/terrain.vert",
                                                             "../resources/shaders/terrain.frag", 5.0f);
    registry.emplace<stratgame::ModelComponent>(terrain_entity, terrain);
    registry.emplace<stratgame::ShaderComponent>(terrain_entity, terrain_shader);
    registry.emplace<stratgame::Transform>(terrain_entity, Vector3{0, 0, 0});
    registry.emplace<stratgame::DrawModelWireframeComponent>(terrain_entity);

    const auto camera_entity = stratgame::create_camera(registry);

    for (auto i = 0; i < 10; i++) {
        create_minion(registry, {static_cast<float>(i * 2), static_cast<float>(i * 2)});
    }

    while (!WindowShouldClose()) {
        // ======================================
        // UPDATE SYSTEMS
        // ======================================
        handle_input(registry);
        stratgame::update_transform(registry);
        stratgame::update_camera(registry);
        // ======================================

        BeginDrawing();

        ClearBackground(RAYWHITE);

        const auto &camera = registry.get<stratgame::Camera>(camera_entity);
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
