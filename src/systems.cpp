#include "systems.hpp"
#include "common_components.hpp"
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

}; // namespace stratgame
