#pragma once
#include "raylib.h"
#include <entt.hpp>

namespace stratgame {

// ===================================
// teams
// ===================================
using team_color_map = std::unordered_map<int, Color>;
void register_team(entt::registry &registry, const Color &color);

// ===================================
// minions
// ===================================
struct Minion {
    int team_id;
};

struct BaseStats {
    int health{100};
    int attack{10};
};

auto create_minion(entt::registry &registry, Vector2 position, int team_id) -> entt::entity;
void update_minion_heights(entt::registry &registry);

} // namespace stratgame
