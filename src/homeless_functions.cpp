#include "homeless_functions.hpp"
#include "assets_loader.hpp"
#include "common_components.hpp"
#include "drawing.hpp"
#include "minion.hpp"
#include <raylib.h>

namespace stratgame {
void setup_raylib() {

    const auto display = GetCurrentMonitor();
    const int screen_width = GetMonitorWidth(display);
    const int screen_height = GetMonitorHeight(display);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(screen_width, screen_height, "RTS game");
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

        const auto team_color_map_entity = registry.view<stratgame::team_color_map>().begin()[0];
        const auto &team_colors = registry.get<stratgame::team_color_map>(team_color_map_entity);
        const auto color = team_colors.at(minion.team_id);

        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = color;
        registry.emplace<stratgame::ModelComponent>(entity, model);
    }>();

    registry.on_construct<Selected>().connect<[](entt::registry &registry, entt::entity entity) {
        if (registry.all_of<Minion, ModelComponent>(entity)) {
            auto model = registry.get<ModelComponent>(entity);
            model.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = GREEN;
        };
    }>();

    registry.on_destroy<Selected>().connect<[](entt::registry &registry, entt::entity entity) {
        if (registry.all_of<Minion, ModelComponent>(entity)) {
            auto minion = registry.get<Minion>(entity);
            auto model = registry.get<ModelComponent>(entity);
            const auto team_color_map_entity = registry.view<stratgame::team_color_map>().begin()[0];
            const auto &team_colors = registry.get<stratgame::team_color_map>(team_color_map_entity);
            const auto color = team_colors.at(minion.team_id);
            model.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = color;
        };
    }>();

    return registry;
}

void setup_tree(entt::registry & /*registry*/) {
    const auto tree_model = stratgame::load_asset(LoadModel, "tree/tree.gltf");
    const auto tree_instancing_shader =
        stratgame::load_asset(LoadShader, "shaders/instancing.vs", "shaders/instancing.fs");
    tree_instancing_shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(tree_instancing_shader, "mvp");
    tree_instancing_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(tree_instancing_shader, "viewPos");
    tree_instancing_shader.locs[SHADER_LOC_MATRIX_MODEL] =
        GetShaderLocationAttrib(tree_instancing_shader, "instanceTransform");

    for (auto i = 0; i < tree_model.materialCount; i++) {
        tree_model.materials[i].shader = tree_instancing_shader;
        tree_model.materials[i].maps[MATERIAL_MAP_ALBEDO].texture =
            stratgame::load_asset(LoadTexture, "tree/treeDiffuse.png");
    }
    // auto tree_model_entity = stratgame::register_instanceable_model(registry, tree_model);
    //
    // for (int i = 0; i < 50; i++) {
    //     for (int j = 0; j < 50; j++) {
    //         const auto height = heights[4 * i + 4 * j * 200];
    //         const auto x_offset = ((float)rand() / RAND_MAX) * 2 - 1;
    //         const auto z_offset = ((float)rand() / RAND_MAX) * 2 - 1;
    //         const auto tree_entity = registry.create();
    //         stratgame::create_model_instance(registry, tree_model_entity,
    //                                          Vector3{4 * (float)i + x_offset, height, 4 * (float)j + z_offset},
    //                                          tree_entity);
    //     }
    // }
}
} // namespace stratgame
