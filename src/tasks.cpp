#include "tasks.hpp"
#include "common.hpp"
#include "common_components.hpp"
#include "minion.hpp"
#include "terrain.hpp"
#include <raymath.h>
#include <print>

namespace stratgame {

void add_task(entt::registry &registry, const entt::entity entity, const Task& task) {
    if (!registry.all_of<TaskQueue>(entity)) {
        registry.emplace<TaskQueue>(entity);
    }

    std::println("Adding task to entity");

    registry.patch<TaskQueue>(entity, [&](TaskQueue &task_queue) { task_queue.set_new_task(task); });
}

auto handle_walk_to_task(Transform &transform, const WalkToTask &task, float delta) -> TaskStatus {
    const auto target = to_vec3(task.target);

    const auto diff_to_target = Vector3Subtract(target, transform.position);
    const auto diff_to_target2d = to_vec2(diff_to_target);
    const auto direction = Vector2Normalize(diff_to_target2d);
    const auto movement_delta_scalar = task.speed * delta;
    const auto movement_delta = Vector2Scale(direction, movement_delta_scalar);

    if (Vector2Length(diff_to_target2d) < movement_delta_scalar) {
        std::println("Reached target!");
        transform.position = target;
        return TaskStatus::Finished;
    } else {
        transform.position = Vector3Add(transform.position, {movement_delta.x, 0, movement_delta.y});
    }

    return TaskStatus::InProgress;
}

void update_tasks(entt::registry &registry) {
    const auto minions = registry.view<stratgame::Minion, stratgame::Transform, stratgame::TaskQueue>();

    const auto delta = GetFrameTime();

    for (auto minion : minions) {
        auto &transform = registry.get<stratgame::Transform>(minion);
        auto &task_queue = registry.get<TaskQueue>(minion);

        if (task_queue.is_empty()) {
            continue;
        }

        const auto &task = task_queue.get_current_task();

        TaskStatus status = TaskStatus::InProgress;
        std::visit(
            overloaded{
				[&](const WalkToTask &task) { status = handle_walk_to_task(transform, task, delta); },
			},
			task);

        if (status == TaskStatus::Finished) {
			task_queue.remove_task();
		}
    }
}

void tasks_from_input(entt::registry &registry) {
    const auto terrain_entity = registry.view<const stratgame::TerrainClick>().begin()[0];
    const auto &terrain_click = registry.get<const stratgame::TerrainClick>(terrain_entity);
    if (terrain_click.position) {
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            auto selected_minions = registry.view<const stratgame::Minion, const stratgame::Selected>();

            for (auto minion : selected_minions) {
                add_task(registry, minion, stratgame::WalkToTask{*terrain_click.position, 5.f});
            }
        }
    }
}

} // namespace stratgame
