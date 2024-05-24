#include "systems.hpp"
#include "camera.hpp"
#include "common_components.hpp"
#include "drawing.hpp"
#include "minion.hpp"
#include "tasks.hpp"
#include "terrain.hpp"
#include <print>
#include <raylib.h>
#include <raymath.h>
#include "common.hpp"

namespace stratgame {
void update_transform(entt::registry &registry) {
    auto view = registry.view<Movement, Transform>();
    for (auto&& [entity, movement, transform] : view.each()) {
        transform.position = Vector3Add(transform.position, movement.velocity);
        movement.velocity = {0.,0.,0.};
    }
}

void update_context(entt::registry &registry) {}

void handle_input(entt::registry &registry) {
    handle_mouse_input(registry);
    handle_camera_input(registry);
    tasks_from_input(registry);
}

void handle_mouse_input(entt::registry &registry) {
    if (!(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))) {
        return;
    }

    const auto camera_entity = registry.view<Camera>().begin()[0];
    const auto &camera = registry.get<Camera>(camera_entity);
    const auto terrain_entity = registry.view<TerrainClick>().begin()[0];
    const auto mouse_pos = GetMousePosition();
    const auto mouse_to_model_ray = GetMouseRay(mouse_pos, camera.camera3d);
    const auto model_entities = registry.view<ModelComponent, Transform>();

    for (auto &&[entity, model_component, transform] : model_entities.each()) {
        if (!model_component.visible) {
            continue;
        }

        const auto hit =
            GetRayCollisionMesh(mouse_to_model_ray, model_component.model.meshes[0],
                                MatrixTranslate(transform.position.x, transform.position.y, transform.position.z));
        if (hit.hit) {
            std::println("hit model at {}, {}, {}", hit.point.x, hit.point.y, hit.point.z);
            registry.patch<TerrainClick>(
                terrain_entity, [&](TerrainClick &click) { click.position = std::optional{to_vec2(hit.point)}; });
            break;
        }
    }

    const auto minions = registry.view<Minion, ModelComponent, Transform>();
    for (auto minion : minions) {
        const auto &transform = registry.get<Transform>(minion);

        const auto minion_hit = GetRayCollisionSphere(mouse_to_model_ray, transform.position, 1.f);

        if (minion_hit.hit) {
            auto selected_minions = registry.view<Selected>();
            if (!IsKeyDown(KEY_LEFT_SHIFT)) {
                for (auto selected_minion : selected_minions) {
                    registry.erase<Selected>(selected_minion);
                }
            }
            registry.emplace_or_replace<Selected>(minion);
            break;
            // registry.patch<Selectable>(minion, [&](Selectable &selectable) {
            //     selectable.selected = !selectable.selected;

            //     registry.patch<SelectedState>(registry.view<SelectedState>().begin()[0],
            //                                   [&](SelectedState &selected) {
            //                                       selected.selected_entities_count += selectable.selected ? 1 :
            //                                       -1;
            //                                   });
            // });
        }
    }
}
}; // namespace stratgame
