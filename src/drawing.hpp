#pragma once

#include <entt.hpp>
#include <raylib.h>

namespace stratgame {
struct ModelComponent {
    Model model;
    Vector3 position;
};

struct ShaderComponent {
    Shader shader;
};

// requires ModelComponent
struct DrawModelWireframeComponent {};

void draw_models(entt::registry &registry);
void draw_model_wireframes(entt::registry &registry);
}; // namespace stratgame
