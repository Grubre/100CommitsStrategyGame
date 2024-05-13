#include "systems.hpp"
#include "camera.hpp"
#include "common_components.hpp"
#include "drawing.hpp"
#include "minion.hpp"
#include "tasks.hpp"
#include "terrain.hpp"
#include <raylib.h>
#include <raymath.h>
#include "common.hpp"

namespace stratgame {
void update_transform(entt::registry &registry) {
    auto view = registry.view<const Movement, Transform>();
    for (auto entity : view) {
        const auto &movement = view.get<Movement>(entity);
        auto &transform = view.get<Transform>(entity);

        transform.position = Vector3Add(transform.position, movement.velocity);
    }
}

void handle_input(entt::registry &registry) {
    // handle_mouse_input(registry);
    handle_camera_input(registry);
    tasks_from_input(registry);
}

void handle_mouse_input(entt::registry &registry) {
    const auto terrain_entity = registry.view<TerrainClick>().begin()[0];
    const auto &terrain = registry.get<ModelComponent>(terrain_entity);
    const auto camera_entity = registry.view<Camera>().begin()[0];
    const auto &camera = registry.get<Camera>(camera_entity);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        const auto mouse_pos = GetMousePosition();
        const auto ray = GetMouseRay(mouse_pos, camera.camera3d);
        // check hit between terrain and ray
        const auto hit = GetRayCollisionMesh(ray, terrain.model.meshes[0], terrain.model.transform);

        registry.patch<TerrainClick>(terrain_entity, [&](TerrainClick &click) {
            click.position = hit.hit ? std::optional{to_vec2(hit.point)} : std::nullopt;
        });

        auto minions = registry.view<Minion, ModelComponent, Transform>();
        for (auto minion : minions) {
            const auto &transform = registry.get<Transform>(minion);

            const auto minion_hit = GetRayCollisionSphere(ray, transform.position, 1.f);

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
                //                                       selected.selected_entities_count += selectable.selected ? 1 : -1;
                //                                   });
                // });
            }
        }
    }
}

}; // namespace stratgame
