#include "systems.hpp"
#include "camera.hpp"
#include "common_components.hpp"
#include "drawing.hpp"
#include "minion.hpp"
#include "tasks.hpp"
#include "terrain.hpp"
#include <raylib.h>
#include <raymath.h>

namespace stratgame {
void update_transform(entt::registry &registry) {
    auto view = registry.view<const stratgame::Movement, stratgame::Transform>();
    for (auto entity : view) {
        const auto &movement = view.get<stratgame::Movement>(entity);
        auto &transform = view.get<stratgame::Transform>(entity);

        transform.position = Vector3Add(transform.position, movement.velocity);
    }
}

void handle_input(entt::registry &registry) {
    stratgame::handle_mouse_input(registry);
    stratgame::handle_camera_input(registry);
    stratgame::tasks_from_input(registry);
}

void handle_mouse_input(entt::registry &registry) {
    const auto terrain_entity = registry.view<stratgame::TerrainClick>().begin()[0];
    const auto terrain = registry.get<stratgame::ModelComponent>(terrain_entity);
    const auto camera_entity = registry.view<stratgame::Camera>().begin()[0];
    const auto &camera = registry.get<stratgame::Camera>(camera_entity);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        const auto mouse_pos = GetMousePosition();
        const auto ray = GetMouseRay(mouse_pos, camera.camera3d);
        // check hit between terrain and ray
        const auto hit = GetRayCollisionMesh(ray, terrain.model.meshes[0], terrain.model.transform);

        registry.patch<stratgame::TerrainClick>(terrain_entity, [&](stratgame::TerrainClick &click) {
            click.position = hit.hit ? std::optional{Vector2{hit.point.x, hit.point.z}} : std::nullopt;
        });

        auto minions = registry.view<stratgame::Minion, stratgame::ModelComponent, stratgame::Transform>();
        for (auto minion : minions) {
            const auto &transform = registry.get<stratgame::Transform>(minion);

            const auto minion_hit = GetRayCollisionSphere(ray, transform.position, 1.f);

            if (minion_hit.hit) {
                registry.patch<stratgame::Selectable>(minion, [&](stratgame::Selectable &selectable) {
                    selectable.selected = !selectable.selected;

                    registry.patch<SelectedState>(registry.view<SelectedState>().begin()[0],
                                                  [&](SelectedState &selected) {
                                                      selected.selected_entities_count += selectable.selected ? 1 : -1;
                                                  });
                });
            }
        }
    }
}

}; // namespace stratgame
