#pragma once
#include "common_components.hpp"
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <numbers>

namespace stratgame {

enum class ZOOM_DIRECTION { IN, OUT, NONE };

struct Camera {
    Camera3D camera3d;

    explicit Camera(Camera3D c) : camera3d(c) {}

    float max_zoom = 100.f; // max distance from target
    float min_zoom = 10.f;  // min distance from target
    float min_pitch = 0.2f * std::numbers::pi;
    float max_pitch = 0.49f * std::numbers::pi;
    float speed = 30.0f;
    float rotation_speed = 5.f;
    float zoom_speed = 1000.f;

    float zoom = 20.0f;      // distance from target
    float yaw = 0.f;        // rotation around y axis
    float pitch = std::numbers::pi / 3.f; // angle between xz plane and camera
    Vector2 target_position = Vector2{0.0, 0.0};
    
    // ZOOM_DIRECTION zoom_dir = ZOOM_DIRECTION::NONE;

    [[nodiscard]] auto get_target_position() const -> Vector3 {
        return Vector3{.x = target_position.x, .y = 0.0f, .z = target_position.y};
    }
    [[nodiscard]] auto get_source_position() const -> Vector3 {
        auto const pitched = Vector3 { cos(pitch), sin(pitch), 0.0f };
        auto const yawed = Vector3RotateByAxisAngle(pitched, Vector3{0.0,1.0,0.0}, yaw);
        auto const source_offset = Vector3Scale(yawed, zoom);

        auto const source_vec = Vector3Add(source_offset, get_target_position());

        return source_vec;
    }
    [[nodiscard]] auto is_within_zoom_bounds() const -> bool { return zoom <= max_zoom && zoom >= min_zoom; }

    void keep_rotation_bounds() { 
        pitch = std::clamp(pitch, min_pitch, max_pitch);
        yaw = std::fmod(yaw, 2 * std::numbers::pi);
    }
    void keep_zoom_bounds() {
        zoom = std::clamp(zoom, min_zoom, max_zoom);
    }

    
};

auto create_camera(entt::registry &registry) -> entt::entity;

void handle_camera_input(entt::registry &registry);
void update_camera(entt::registry &registry);
}; // namespace stratgame
