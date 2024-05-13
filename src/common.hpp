#pragma once
#include <raymath.h>

namespace stratgame {
[[nodiscard]] inline auto to_vec3(const Vector2 &v, const float y = 0.f) -> Vector3 {
    return Vector3{v.x, y, v.y};
}

[[nodiscard]] inline auto to_vec2(const Vector3 &v) -> Vector2 {
    return Vector2{v.x, v.z};
}
} // namespace stratgame
