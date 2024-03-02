#include <iostream>
#include <entt.hpp>

int main() {
    entt::registry registry;

    auto entity = registry.create();
    registry.emplace<std::string>(entity, "Hello, World!");

    const auto& message = registry.get<std::string>(entity);
    std::cout << message << std::endl;

    return 0;
}

