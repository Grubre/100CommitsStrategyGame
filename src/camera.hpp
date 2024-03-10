#pragma once
#include "common_components.hpp"

namespace stratgame {

struct Camera {
    Camera3D camera3d;
    bool active;
};

auto create_camera(entt::registry &registry) -> entt::entity {
    auto entity = registry.create();
    registry.emplace<Camera3D>(entity, Camera3D{.position = Vector3{15.0f, 25.0f, 15.0f},
                                                .target = Vector3{0.0f, 0.0f, 0.0f},
                                                .up = Vector3{0.0f, 1.0f, 0.0f},
                                                .fovy = 45.0f,
                                                .projection = CAMERA_PERSPECTIVE});
    registry.emplace<Movement>(entity, Movement{.velocity = {0, 0, 0}, .speed = 1.0f});
    return entity;
}

}; // namespace stratgame
