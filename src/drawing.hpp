#pragma once

#include <entt.hpp>
#include <raylib.h>

namespace stratgame {
struct ModelComponent {
    explicit ModelComponent(Model model, float scale = 1.0f) : model{model}, scale{scale} {}

    Model model;
    float scale;
    bool visible = true;
};

struct ShaderComponent {
    Shader shader;
};

struct FrustumCullingComponent {
    float radius;
    Vector2 offset; // offset from the Transform component used in the frustum culling check
};
void flag_culled_models(entt::registry &registry);

// requires ModelComponent
struct DrawModelWireframeComponent {};

void draw_models(const entt::registry &registry);
void draw_model_wireframes(const entt::registry &registry);
void draw_models_instanced(entt::registry &registry);

struct InstanceableModel {
    int model_id;
    Model model;
    std::vector<Matrix> transforms;
};

struct ModelInstance {
    int model_id;
    int instance_id;
};

auto register_instanceable_model(entt::registry &registry, const Model &model) -> entt::entity;
void create_model_instance(entt::registry &registry, entt::entity model_entity, Vector3 transform,
                           entt::entity object_entity);
}; // namespace stratgame
