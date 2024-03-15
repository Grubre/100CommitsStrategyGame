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

struct Transform {
    Vector3 position;
};

using Position = Vector3;
}; // namespace stratgame
