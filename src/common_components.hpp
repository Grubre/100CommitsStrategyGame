#pragma once
#include "raylib.h"
#include "raymath.h"
#include <entt.hpp>

using Velocity = Vector3;

struct Movement {
    Vector3 velocity;
    float speed;
};
