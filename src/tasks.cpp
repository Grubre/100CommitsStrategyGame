#include "tasks.hpp"
#include "common_components.hpp"
#include "minion.hpp"
#include <iostream>
#include <raymath.h>

namespace stratgame {

void add_task(entt::registry &registry, entt::entity entity, Task task) {
    if (!registry.all_of<TaskQueue>(entity)) {
        registry.emplace<TaskQueue>(entity);
    }

    std::cout << "Adding task to entity\n";

    registry.patch<TaskQueue>(entity, [&](TaskQueue &task_queue) { task_queue.set_new_task(task); });
}

void update_tasks(entt::registry &registry) {
    auto minions = registry.view<stratgame::Minion, stratgame::Transform, stratgame::TaskQueue>();

    const auto delta = GetFrameTime();

    for (auto minion : minions) {
        auto &transform = registry.get<stratgame::Transform>(minion);
        auto &task_queue = registry.get<TaskQueue>(minion);

        if (task_queue.empty()) {
            continue;
        }

        const auto &task = task_queue.current_task();

        if (std::holds_alternative<WalkToTask>(task)) {
            const auto walk_to_task = std::get<WalkToTask>(task);
            const auto target = Vector3{walk_to_task.target.x, 0, walk_to_task.target.y};

            const auto diff_to_target = Vector3Subtract(target, transform.position);
            const auto diff_to_target2d = Vector2{diff_to_target.x, diff_to_target.z};
            const auto direction = Vector2Normalize(diff_to_target2d);
            const auto movement_delta_scalar = walk_to_task.speed * delta;
            const auto movement_delta = Vector2Scale(direction, movement_delta_scalar);

            if (Vector2Length(diff_to_target2d) < movement_delta_scalar) {
                std::cout << "Reached target!\n";
                transform.position = target;
                task_queue.remove_task();
            } else {
                transform.position = Vector3Add(transform.position, {movement_delta.x, 0, movement_delta.y});
            }
        }
    }
}

} // namespace stratgame
