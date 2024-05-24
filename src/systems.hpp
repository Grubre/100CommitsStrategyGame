#pragma once
#include <entt.hpp>

namespace stratgame {
void update_transform(entt::registry &registry);
void update_context(entt::registry &registry);

void handle_input(entt::registry &registry);
void handle_mouse_input(entt::registry &registry);

}; // namespace stratgame
