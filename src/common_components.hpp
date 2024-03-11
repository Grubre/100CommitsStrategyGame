#pragma once
#include "raylib.h"
#include "raymath.h"
#include <entt.hpp>

namespace stratgame {

using Velocity = Vector3;

struct Movement {
    Vector3 velocity;
    float speed;
};

using Position = Vector3;
}; // namespace stratgame
