#pragma once
#include "common_components.hpp"

namespace stratgame {

enum class ZOOM_DIRECTION { IN, OUT, NONE };

struct Camera {
    Camera3D camera3d;
    bool active;
    float max_zoom = 100.f; // distance from target
    float min_zoom = 10.f;  // distance from target
    float rotation = PI / 4.f;
    ZOOM_DIRECTION zoom = ZOOM_DIRECTION::NONE;

    [[nodiscard]] auto get_zoom() const -> float { return Vector3Distance(camera3d.position, camera3d.target); }
    [[nodiscard]] auto is_within_zoom_bounds() const -> bool {
        return get_zoom() <= max_zoom && get_zoom() >= min_zoom;
    }
};

auto create_camera(entt::registry &registry) -> entt::entity;

void handle_camera_input(entt::registry &registry);
void update_camera(entt::registry &registry);
}; // namespace stratgame
