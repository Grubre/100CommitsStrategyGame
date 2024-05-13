#pragma once
#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <numbers>
#include "common.hpp"
#include "common_components.hpp"
#include <raymath.h>

namespace stratgame {

enum class ZOOM_DIRECTION { IN, OUT, NONE };

struct Camera {
    Camera3D camera3d;

    explicit Camera(Camera3D c) : camera3d(c) {}

    float max_zoom = 100.f; // max distance from target
    float min_zoom = 10.f;  // min distance from target
    float min_pitch = 0.2f * std::numbers::pi_v<float>;
    float max_pitch = 0.49f * std::numbers::pi_v<float>;
    float speed = 30.0f;
    float rotation_speed = 5.f;
    float zoom_speed = 1000.f;

    float zoom = 20.0f;                            // distance from target
    float yaw = 0.f;                               // rotation around y axis
    float pitch = std::numbers::pi_v<float> / 3.f; // angle between xz plane and camera
    Vector2 target_position = Vector2{0.0, 0.0};

    // ZOOM_DIRECTION zoom_dir = ZOOM_DIRECTION::NONE;

    [[nodiscard]] auto get_camera_dir() const -> Vector3 {
        return Vector3Normalize(Vector3Subtract(get_target_position(), get_source_position()));
    }
    [[nodiscard]] auto get_right_vec() const -> Vector3 {
        return Vector3Normalize(Vector3CrossProduct(camera3d.up, get_camera_dir()));
    }
    [[nodiscard]] auto get_up_vec() const -> Vector3 {
        return Vector3Normalize(Vector3CrossProduct(get_camera_dir(), get_right_vec()));
    }

    [[nodiscard]] auto get_target_position() const -> Vector3 { return to_vec3(target_position); }
    [[nodiscard]] auto get_source_position() const -> Vector3 {
        auto const pitched = Vector3{cos(pitch), sin(pitch), 0.0f};
        auto const yawed = Vector3RotateByAxisAngle(pitched, Vector3{0.0, 1.0, 0.0}, yaw);
        auto const source_offset = Vector3Scale(yawed, zoom);

        auto const source_vec = Vector3Add(source_offset, get_target_position());

        return source_vec;
    }
    [[nodiscard]] auto is_within_zoom_bounds() const -> bool { return zoom <= max_zoom && zoom >= min_zoom; }
    [[nodiscard]] auto get_fovx() const -> float {
        return camera3d.fovy * (static_cast<float>(GetScreenWidth()) / static_cast<float>(GetScreenHeight()));
    }

    void keep_rotation_bounds() {
        pitch = std::clamp(pitch, min_pitch, max_pitch);
        yaw = std::fmod(yaw, 2 * std::numbers::pi_v<float>);
    }
    void keep_zoom_bounds() { zoom = std::clamp(zoom, min_zoom, max_zoom); }
};

[[nodiscard]] auto create_camera(entt::registry &registry) -> entt::entity;

void handle_camera_input(entt::registry &registry);
void update_camera(entt::registry &registry);
}; // namespace stratgame
