#include "minion.hpp"
#include "common_components.hpp"
#include "terrain.hpp"

namespace stratgame {

auto create_minion(entt::registry &registry, Vector2 position, int team_id) -> entt::entity {
    const auto entity = registry.create();
    registry.emplace<stratgame::Minion>(entity, team_id);

    auto &transform = registry.get<stratgame::Transform>(entity);
    transform.position.x = position.x;
    transform.position.z = position.y;
    return entity;
}

void update_minion_heights(entt::registry &registry) {
    const auto height_entity = registry.view<const stratgame::GeneratedTerrain::Heights>();
    const auto &heights = registry.get<stratgame::GeneratedTerrain::Heights>(*height_entity.begin());

    auto minions = registry.view<stratgame::Minion>();

    for (auto minion : minions) {
        auto &transform = registry.get<stratgame::Transform>(minion);

        const auto x = static_cast<int>(transform.position.x);
        const auto z = static_cast<int>(transform.position.z);

        // NOTE: The +1 is because minions are temporarily spheres of radius 1
        transform.position.y = heights[z * 200 + x] + 1.f;
    }
}

void register_team(entt::registry &registry, const Color &color) {
    static int team_id = 0;

    if (team_id == 0) {
        const auto team_color_map_entity = registry.create();
        registry.emplace<team_color_map>(team_color_map_entity);
    }

    auto &team_colors = registry.get<team_color_map>(*registry.view<team_color_map>().begin());
    team_colors[team_id] = color;

    team_id++;
}

} // namespace stratgame
