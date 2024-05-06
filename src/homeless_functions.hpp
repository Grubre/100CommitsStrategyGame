#pragma once
#include <entt.hpp>

namespace stratgame {
void setup_raylib();
[[nodiscard]] auto setup_entt() -> entt::registry;

}; // namespace stratgame
