#include "camera.hpp"
#include <iostream>
#include <raylib.h>

namespace stratgame {

constexpr static auto default_raylib_camera = Camera3D{.position = Vector3{15.0f, 25.0f, 0.f},
                                                       .target = Vector3{0.0f, 0.0f, 0.0f},
                                                       .up = Vector3{0.0f, 1.0f, 0.0f},
                                                       .fovy = 45.0f,
                                                       .projection = CAMERA_PERSPECTIVE};

auto create_camera(entt::registry &registry) -> entt::entity {
    auto entity = registry.create();
    const auto raylib_camera = default_raylib_camera;
    registry.emplace<stratgame::Camera>(entity, raylib_camera, true, 100.0f, 10.0f);
    registry.emplace<stratgame::Transform>(entity, raylib_camera.position);
    return entity;
}

void handle_camera_input(entt::registry &registry) {
    auto view = registry.view<stratgame::Camera>();
    for (auto entity : view) {
        auto &camera = view.get<stratgame::Camera>(entity);

        const auto delta_time = GetFrameTime();
        const auto rotation_amount = 0.05f * delta_time * camera.rotation_speed;
        if (IsKeyDown(KEY_W)) {
            camera.velocity.x -= 1.f;
        }
        if (IsKeyDown(KEY_S)) {
            camera.velocity.x += 1.f;
        }
        if (IsKeyDown(KEY_A)) {
            camera.velocity.y += 1.f;
        }
        if (IsKeyDown(KEY_D)) {
            camera.velocity.y -= 1.f;
        }
        if (IsKeyDown(KEY_LEFT)) {
            camera.rotation += rotation_amount;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            camera.rotation -= rotation_amount;
        }

        camera.velocity = Vector2Rotate(camera.velocity, camera.rotation);

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            auto mouse_delta = GetMouseDelta();
            mouse_delta = Vector2Rotate(mouse_delta, -camera.rotation);
            camera.velocity.x -= mouse_delta.x;
            camera.velocity.y -= mouse_delta.y;
        }

        camera.velocity = Vector2Normalize(camera.velocity);
        camera.velocity = Vector2Scale(camera.velocity, delta_time);
        camera.velocity = Vector2Scale(camera.velocity, camera.speed);

        const auto scroll = GetMouseWheelMove();
        camera.zoom = scroll > 0   ? ZOOM_DIRECTION::IN
                                : scroll < 0 ? ZOOM_DIRECTION::OUT
                                             : ZOOM_DIRECTION::NONE;
    }
}

void update_camera(entt::registry &registry) {
    auto view = registry.view<stratgame::Camera, stratgame::Transform>();
    for (auto entity : view) {
        auto &transform = view.get<stratgame::Transform>(entity);
        auto &camera = view.get<stratgame::Camera>(entity);
        auto &camera3d = camera.camera3d;

        // check if the camera is within the zoom bounds
        auto old_position = transform.position;
        if (camera.zoom == ZOOM_DIRECTION::IN) {
            transform.position = Vector3Subtract(
                transform.position, Vector3Scale(Vector3Subtract(transform.position, camera3d.target), 0.1f));
        } else if (camera.zoom == ZOOM_DIRECTION::OUT) {
            transform.position =
                Vector3Add(transform.position, Vector3Scale(Vector3Subtract(transform.position, camera3d.target), 0.1f));
        }
        transform.position = camera.is_within_zoom_bounds() ? transform.position : old_position;

        const auto velocity_vec3 = Vector3{camera.velocity.x, 0, camera.velocity.y};

        // move the target by the velocity
        camera3d.target = Vector3Add(camera3d.target, velocity_vec3);

        // move the transform by the velocity
        transform.position = Vector3Add(transform.position, velocity_vec3);

        // Rotate transform around the target in the xz plane
        auto xz_position = Vector2{transform.position.x, transform.position.z};
        xz_position = Vector2Subtract(xz_position, Vector2{camera3d.target.x, camera3d.target.z});
        xz_position = Vector2Rotate(xz_position, camera.rotation);
        xz_position = Vector2Add(xz_position, Vector2{camera3d.target.x, camera3d.target.z});

        // set the camera position to the transform
        camera3d.position = {xz_position.x, transform.position.y, xz_position.y};

        camera.velocity = {0, 0};
        camera.zoom = ZOOM_DIRECTION::NONE;
    }
}

} // namespace stratgame
