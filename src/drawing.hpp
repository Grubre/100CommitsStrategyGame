#pragma once

#include <entt.hpp>
#include <raylib.h>

namespace stratgame {
struct ModelComponent {
    Model model;
};

struct ShaderComponent {
    Shader shader;
};

// requires ModelComponent
struct DrawModelWireframeComponent {};

void draw_models(entt::registry &registry);
void draw_model_wireframes(entt::registry &registry);

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
void add_instance(entt::registry &registry, entt::entity model_entity, entt::entity object_entity);
void draw_models_instanced(entt::registry &registry);
}; // namespace stratgame
