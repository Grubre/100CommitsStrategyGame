#pragma once
#include <raylib.h>
#include <raymath.h>

template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};

namespace stratgame {
[[nodiscard]] inline auto to_vec3(const Vector2 &v, const float y = 0.f) -> Vector3 {
    return Vector3{v.x, y, v.y};
}

[[nodiscard]] inline auto to_vec2(const Vector3 &v) -> Vector2 {
    return Vector2{v.x, v.z};
}

[[nodiscard]] inline auto get_aspect_ratio() -> float {
    return static_cast<float>(GetScreenWidth()) / static_cast<float>(GetScreenHeight());
}
} // namespace stratgame
