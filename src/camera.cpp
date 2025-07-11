#include "camera.hpp"
#include <raylib.h>
#include <raymath.h>

namespace stratgame {

constexpr static auto default_raylib_camera = Camera3D{.position = Vector3{0.0f, 0.0f, 0.f},
                                                       .target = Vector3{0.0f, 0.0f, 0.0f},
                                                       .up = Vector3{0.0f, 1.0f, 0.0f},
                                                       .fovy = 90.0f,
                                                       .projection = CAMERA_PERSPECTIVE};

auto create_camera(entt::registry &registry) -> entt::entity {
    const auto entity = registry.create();
    const auto raylib_camera = default_raylib_camera;
    registry.emplace<stratgame::Camera>(entity, raylib_camera);
    return entity;
}

void handle_camera_input(entt::registry &registry) {
    const auto view = registry.view<stratgame::Camera>();
    for (auto entity : view) {
        auto &camera = view.get<stratgame::Camera>(entity);

        const auto delta_time = GetFrameTime();

        auto pitch_velocity = 0.f;
        auto yaw_velocity = 0.f;
        auto xz_velocity = Vector2{0.0, 0.0};
        if (IsKeyDown(KEY_W)) {
            xz_velocity.x -= 1.f;
        }
        if (IsKeyDown(KEY_S)) {
            xz_velocity.x += 1.f;
        }
        if (IsKeyDown(KEY_A)) {
            xz_velocity.y += 1.f;
        }
        if (IsKeyDown(KEY_D)) {
            xz_velocity.y -= 1.f;
        }
        if (IsKeyDown(KEY_LEFT)) {
            yaw_velocity -= 1.f;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            yaw_velocity += 1.f;
        }
        if (IsKeyDown(KEY_DOWN)) {
            pitch_velocity -= 1.f;
        }
        if (IsKeyDown(KEY_UP)) {
            pitch_velocity += 1.f;
        }

        xz_velocity = Vector2Normalize(xz_velocity);
        xz_velocity = Vector2Rotate(xz_velocity, -camera.yaw);
        xz_velocity = Vector2Scale(xz_velocity, camera.speed * delta_time);
        camera.target_position = Vector2Add(xz_velocity, camera.target_position);

        pitch_velocity = pitch_velocity * delta_time * camera.rotation_speed;
        camera.pitch += pitch_velocity;

        yaw_velocity = yaw_velocity * delta_time * camera.rotation_speed;
        camera.yaw += yaw_velocity;

        const auto scroll = GetMouseWheelMove();
        camera.zoom -= scroll * camera.zoom_speed * 0.01f;

        if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
            const auto mouse_delta = GetMouseDelta();
            camera.yaw -= mouse_delta.x * camera.rotation_speed * 0.001f;
            camera.pitch += mouse_delta.y * camera.rotation_speed * 0.001f;
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            const auto mouse_pos_a = GetMousePosition();
            const auto ray_a = GetMouseRay(mouse_pos_a, camera.camera3d);

            const auto t_a = -ray_a.position.y / ray_a.direction.y;
            const auto terrain_intersection_a =
                Vector2{ray_a.position.x + ray_a.direction.x * t_a, ray_a.position.z + ray_a.direction.z * t_a};

            const auto mouse_delta = GetMouseDelta();
            const auto mouse_pos_b = Vector2Subtract(mouse_pos_a, mouse_delta);
            const auto ray_b = GetMouseRay(mouse_pos_b, camera.camera3d);

            const auto t_b = -ray_b.position.y / ray_b.direction.y;
            const auto terrain_intersection_b =
                Vector2{ray_b.position.x + ray_b.direction.x * t_b, ray_b.position.z + ray_b.direction.z * t_b};

            const auto delta = Vector2Subtract(terrain_intersection_b, terrain_intersection_a);

            camera.target_position = Vector2Add(camera.target_position, delta);
        }
    }
}

void update_camera(entt::registry &registry) {
    const auto view = registry.view<stratgame::Camera>();
    for (auto entity : view) {
        auto &camera = view.get<stratgame::Camera>(entity);
        auto &camera3d = camera.camera3d;

        camera.keep_rotation_bounds();
        camera.keep_zoom_bounds();

        camera3d.target = camera.get_target_position();
        camera3d.position = camera.get_source_position();
    }
}

} // namespace stratgame
