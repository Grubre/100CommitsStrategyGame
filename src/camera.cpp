#include "camera.hpp"
#include <iostream>
#include <raylib.h>

namespace stratgame {

constexpr static auto default_raylib_camera = Camera3D{.position = Vector3{15.0f, 25.0f, 15.0f},
                                                       .target = Vector3{0.0f, 0.0f, 0.0f},
                                                       .up = Vector3{0.0f, 1.0f, 0.0f},
                                                       .fovy = 45.0f,
                                                       .projection = CAMERA_PERSPECTIVE};

auto create_camera(entt::registry &registry) -> entt::entity {
    auto entity = registry.create();
    const auto raylib_camera = default_raylib_camera;
    registry.emplace<stratgame::Camera>(entity, raylib_camera, true, 100.0f, 10.0f);
    registry.emplace<stratgame::Movement>(entity, stratgame::Movement{.velocity = {0, 0, 0}, .speed = 1.0f});
    return entity;
}

void handle_camera_input(entt::registry &registry) {
    auto view = registry.view<stratgame::Camera, stratgame::Movement>();
    for (auto entity : view) {
        auto &movement = view.get<stratgame::Movement>(entity);
        auto &camera_component = view.get<stratgame::Camera>(entity);
        if (IsKeyDown(KEY_W)) {
            movement.velocity.x -= 1.f;
        }
        if (IsKeyDown(KEY_S)) {
            movement.velocity.x += 1.f;
        }
        if (IsKeyDown(KEY_A)) {
            movement.velocity.z += 1.f;
        }
        if (IsKeyDown(KEY_D)) {
            movement.velocity.z -= 1.f;
        }
        auto xz_velocity = Vector2{movement.velocity.x, movement.velocity.z};
        xz_velocity = Vector2Rotate(xz_velocity, camera_component.rotation);
        movement.velocity = {xz_velocity.x, movement.velocity.y, xz_velocity.y};

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            auto mouse_delta = GetMouseDelta();
            mouse_delta = Vector2Rotate(mouse_delta, -camera_component.rotation);
            movement.velocity.x -= mouse_delta.x;
            movement.velocity.z -= mouse_delta.y;
        }

        const auto scroll = GetMouseWheelMove();
        camera_component.zoom = scroll > 0   ? ZOOM_DIRECTION::IN
                                : scroll < 0 ? ZOOM_DIRECTION::OUT
                                             : ZOOM_DIRECTION::NONE;
    }
}

void update_camera(entt::registry &registry) {
    auto view = registry.view<stratgame::Camera, stratgame::Movement>();
    for (auto entity : view) {
        auto &movement = view.get<stratgame::Movement>(entity);
        auto &camera_component = view.get<stratgame::Camera>(entity);
        auto &camera = camera_component.camera3d;

        const auto delta_time = GetFrameTime();
        const auto scaled_velocity = Vector3Scale(movement.velocity, delta_time * 50.f);

        auto old_position = camera.position;
        if (camera_component.zoom == ZOOM_DIRECTION::IN) {
            camera.position =
                Vector3Subtract(camera.position, Vector3Scale(Vector3Subtract(camera.position, camera.target), 0.1f));
        } else if (camera_component.zoom == ZOOM_DIRECTION::OUT) {
            camera.position =
                Vector3Add(camera.position, Vector3Scale(Vector3Subtract(camera.position, camera.target), 0.1f));
        }

        camera.position = camera_component.is_within_zoom_bounds() ? camera.position : old_position;
        camera.position = Vector3Add(camera.position, scaled_velocity);
        camera.target = Vector3Add(camera.target, scaled_velocity);

        movement.velocity = {0, 0, 0};
        camera_component.zoom = ZOOM_DIRECTION::NONE;
    }
}

} // namespace stratgame
