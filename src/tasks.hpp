#pragma once

#include <deque>
#include <entt.hpp>
#include <raylib.h>
#include <variant>

namespace stratgame {
struct WalkToTask {
    Vector2 target;
    float speed;
};

using Task = std::variant<WalkToTask>;

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

    [[nodiscard]] auto tasks() const -> const std::deque<Task> & { return m_tasks; }
    [[nodiscard]] auto current_task() const -> const Task & { return m_tasks[0]; }
    [[nodiscard]] auto empty() const -> bool { return m_tasks.empty(); }

  private:
    std::deque<Task> m_tasks;
};

void add_task(entt::registry &registry, entt::entity entity, Task task);
void update_tasks(entt::registry &registry);

} // namespace stratgame
