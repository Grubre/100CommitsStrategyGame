#include "drawing.hpp"
#include "common_components.hpp"
#include <print>
#include <raymath.h>

namespace stratgame {
void draw_models(const entt::registry &registry) {
    const auto view = registry.view<ModelComponent, stratgame::Transform>();
    for (auto entity : view) {
        const auto &model_component = view.get<ModelComponent>(entity);
        const auto &transform = view.get<stratgame::Transform>(entity);

        if (registry.any_of<ShaderComponent>(entity)) {
            const auto &shader_component = registry.get<ShaderComponent>(entity);
            const auto shader_backup = model_component.model.materials[0].shader;
            model_component.model.materials[0].shader = shader_component.shader;
            DrawModel(model_component.model, transform.position, 1.0f, WHITE);
            model_component.model.materials[0].shader = shader_backup;
            continue;
        }
        DrawModel(model_component.model, transform.position, model_component.scale, WHITE);
    }
}

auto register_instanceable_model(entt::registry &registry, const Model &model) -> entt::entity {
    static int model_id = 0;

    std::println("Registering model with id: {}", model_id);

    const auto entity = registry.create();
    registry.emplace<InstanceableModel>(entity, model_id, model, std::vector<Matrix>{});

    model_id++;
    return entity;
}

void create_model_instance(entt::registry &registry, entt::entity model_entity, Vector3 transform,
                           entt::entity object_entity) {
    registry.emplace<stratgame::Transform>(object_entity, transform);

    auto &instanceable_model = registry.get<InstanceableModel>(model_entity);
    auto &transforms = instanceable_model.transforms;

    transforms.push_back(MatrixIdentity());
    registry.emplace<ModelInstance>(object_entity, instanceable_model.model_id, static_cast<int>(transforms.size()));
}

void draw_models_instanced(entt::registry &registry) {
    const auto models = registry.view<InstanceableModel>();
    const auto instanced_entities = registry.view<ModelInstance, stratgame::Transform>();

    for (auto model_entity : models) {
        auto &instanceable_model = models.get<InstanceableModel>(model_entity);
        auto &transforms = instanceable_model.transforms;

        for (auto instanced_entity : instanced_entities) {
            const auto &model_instance = instanced_entities.get<ModelInstance>(instanced_entity);
            if (model_instance.model_id != instanceable_model.model_id) {
                continue;
            }

            auto &transform = instanced_entities.get<stratgame::Transform>(instanced_entity);
            transforms[static_cast<std::size_t>(model_instance.instance_id - 1)] =
                MatrixTranslate(transform.position.x, transform.position.y, transform.position.z);
        }

        for (auto i = 0; i < instanceable_model.model.meshCount; i++) {
            DrawMeshInstanced(instanceable_model.model.meshes[i], instanceable_model.model.materials[0],
                              transforms.data(), static_cast<int>(transforms.size()));
        }
    }
}

void draw_model_wireframes(const entt::registry &registry) {
    const auto view = registry.view<ModelComponent, stratgame::Transform, DrawModelWireframeComponent>();
    for (auto entity : view) {
        const auto &model_component = view.get<ModelComponent>(entity);
        const auto &transform = view.get<stratgame::Transform>(entity);
        DrawModelWires(model_component.model, transform.position, 1.0f, Fade(LIGHTGRAY, 0.6f));
    }
}
}; // namespace stratgame
