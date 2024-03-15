#include "drawing.hpp"

namespace stratgame {
void draw_models(entt::registry &registry) {
    auto view = registry.view<ModelComponent>();
    for (auto entity : view) {
        auto &model_component = view.get<ModelComponent>(entity);

        if (registry.any_of<ShaderComponent>(entity)) {
            auto &shader_component = registry.get<ShaderComponent>(entity);
            auto shader_backup = model_component.model.materials[0].shader;
            model_component.model.materials[0].shader = shader_component.shader;
            DrawModel(model_component.model, model_component.position, 1.0f, WHITE);
            model_component.model.materials[0].shader = shader_backup;
            continue;
        }
        DrawModel(model_component.model, model_component.position, 1.0f, WHITE);
    }
}

void draw_model_wireframes(entt::registry &registry) {
    auto view = registry.view<ModelComponent, DrawModelWireframeComponent>();
    for (auto entity : view) {
        auto &model_component = view.get<ModelComponent>(entity);
        DrawModelWires(model_component.model, model_component.position, 1.0f, Fade(LIGHTGRAY, 0.6f));
    }
}
}; // namespace stratgame
