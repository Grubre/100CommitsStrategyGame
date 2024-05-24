#pragma once

#include <deque>
#include <entt.hpp>
#include <raylib.h>
#include <variant>

namespace stratgame {
enum class TaskStatus { InProgress, Finished };

struct WalkToTask {
    Vector2 target;
    float speed;
};

using Task = std::variant<WalkToTask>;

[[nodiscard]] auto handle_walk_to_task(Transform &transform, const WalkToTask &task, float delta) -> TaskStatus;

struct TaskQueue {
    void append_task(Task task) { m_tasks.push_front(task); }
    void remove_task() { m_tasks.pop_front(); }
    void clear_tasks() { m_tasks.clear(); }
    void set_new_task(Task task) {
        if (!m_tasks.empty()) {
            m_tasks[0] = task;
            return;
        }
        m_tasks.push_front(task);
    }

    [[nodiscard]] auto get_tasks() const -> const std::deque<Task> & { return m_tasks; }
    [[nodiscard]] auto get_current_task() const -> const Task & { return m_tasks[0]; }
    [[nodiscard]] auto is_empty() const -> bool { return m_tasks.empty(); }

  private:
    std::deque<Task> m_tasks;
};

void add_task(entt::registry &registry, const entt::entity entity, const Task& task);
void update_tasks(entt::registry &registry);
void tasks_from_input(entt::registry &registry);

} // namespace stratgame
