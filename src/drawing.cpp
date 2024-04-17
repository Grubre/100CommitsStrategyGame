#include "drawing.hpp"
#include "common_components.hpp"

namespace stratgame {
void draw_models(entt::registry &registry) {
    auto view = registry.view<ModelComponent, stratgame::Transform>();
    for (auto entity : view) {
        auto &model_component = view.get<ModelComponent>(entity);
        auto &transform = view.get<stratgame::Transform>(entity);

        if (registry.any_of<ShaderComponent>(entity)) {
            auto &shader_component = registry.get<ShaderComponent>(entity);
            auto shader_backup = model_component.model.materials[0].shader;
            model_component.model.materials[0].shader = shader_component.shader;
            DrawModel(model_component.model, transform.position, 1.0f, WHITE);
            model_component.model.materials[0].shader = shader_backup;
            continue;
        }
        DrawModel(model_component.model, transform.position, 1.0f, WHITE);
    }
}

struct InstanceableModel {
    int model_id;
    Model model;
    std::vector<Matrix> transforms;
};

struct ModelInstance {
    int model_id;
    int instance_id;
};

auto register_instanceable_model(entt::registry &registry, const Model &model) -> entt::entity {
    static int model_id = 0;

    auto entity = registry.create();
    registry.emplace<InstanceableModel>(entity, model_id, model, std::vector<Matrix>{});

    model_id++;
    return entity;
}

void add_instance(entt::registry &registry, entt::entity model_entity) {
    auto &instanceable_model = registry.get<InstanceableModel>(model_entity);
    instanceable_model.transforms.push_back(MatrixIdentity());
    registry.emplace<ModelInstance>(model_entity, instanceable_model.transforms.size());
}

void draw_models_instanced(entt::registry &registry) {
    auto view = registry.view<InstanceableModel>();




}

void draw_model_wireframes(entt::registry &registry) {
    auto view = registry.view<ModelComponent, stratgame::Transform, DrawModelWireframeComponent>();
    for (auto entity : view) {
        auto &model_component = view.get<ModelComponent>(entity);
        auto &transform = view.get<stratgame::Transform>(entity);
        DrawModelWires(model_component.model, transform.position, 1.0f, Fade(LIGHTGRAY, 0.6f));
    }
}
}; // namespace stratgame
