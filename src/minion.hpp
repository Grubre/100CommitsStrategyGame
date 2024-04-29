#pragma once
#include "raylib.h"
#include <entt.hpp>

namespace stratgame {

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
