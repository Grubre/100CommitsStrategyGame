#pragma once
#include <entt.hpp>

namespace stratgame {
void setup_raylib();
[[nodiscard]] auto setup_entt() -> entt::registry;

void setup_tree(entt::registry &registry);

}; // namespace stratgame
