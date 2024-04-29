#pragma once

#include "entt.hpp"
#include "raylib.h"

namespace stratgame {

using Velocity = Vector3;

struct Movement {
    Vector3 velocity;
    float speed;
};

struct Transform {
    Vector3 position;
};

struct Selectable {
    bool selected{false};
};

}; // namespace stratgame
