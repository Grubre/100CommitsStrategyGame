#include "camera.hpp"
#include "raylib.h"
#include "raymath.h"
#include <entt.hpp>
#include <iostream>
#include <span>

#include "common_components.hpp"

void update_movement(entt::registry &registry) {
    auto view = registry.view<stratgame::Movement>();
    for (auto entity : view) {
        auto &movement = view.get<stratgame::Movement>(entity);
        movement.velocity = Vector3Normalize(movement.velocity);
        movement.velocity = Vector3Scale(movement.velocity, movement.speed);
    }
}

void handle_input(entt::registry &registry) { stratgame::handle_camera_input(registry); }

auto generate_terrain(uint32_t rows, uint32_t cols, const std::span<const float> heights, float dist_between_vertices = 2.0f) -> Mesh {
    assert(static_cast<unsigned long>(rows * cols) == heights.size());

    auto mesh = Mesh();

    mesh.triangleCount = static_cast<int>((rows - 1u) * (cols - 1u) * 2u);
    mesh.vertexCount = static_cast<int>(rows * cols);
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.indices = (unsigned short *)MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short));

    for (uint32_t i = 0; i < rows; i++) {
        for (uint32_t j = 0; j < cols; j++) {
            const auto index = i * cols + j;
            mesh.vertices[index * 3] = j * dist_between_vertices;
            mesh.vertices[index * 3 + 1] = heights[index];
            mesh.vertices[index * 3 + 2] = i * dist_between_vertices;
        }
    }

    int k = 0;
    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < cols - 1; j++) {
            mesh.indices[k] = i * cols + j;
            mesh.indices[k + 1] = (i + 1) * cols + j;
            mesh.indices[k + 2] = (i + 1) * cols + (j + 1);
            mesh.indices[k + 3] = i * cols + j;
            mesh.indices[k + 4] = (i + 1) * cols + (j + 1);
            mesh.indices[k + 5] = i * cols + (j + 1);
            k += 6; // next quad
        }
    }

    UploadMesh(&mesh, false);

    return mesh;
}

auto main() -> int {
    const int screen_width = 800;
    const int screen_height = 450;
    const int fps = 60;

    entt::registry registry;

    const auto camera_entity = stratgame::create_camera(registry);

    InitWindow(screen_width, screen_height, "Hello World!");
    SetTargetFPS(fps);

    constexpr auto rows = 10u;
    constexpr auto cols = 10u;
    constexpr auto heights = std::array<float, rows * cols>{0.0f};
    auto terrain_mesh = generate_terrain(rows,cols, heights, 2.0f);

    auto terrain = LoadModelFromMesh(terrain_mesh);

    while (!WindowShouldClose()) {
        handle_input(registry);

        update_movement(registry);

        stratgame::update_camera(registry);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        const auto &camera = registry.get<stratgame::Camera>(camera_entity);
        BeginMode3D(camera.camera3d);

        DrawModel(terrain, {0, 0, 0}, 1.0f, WHITE);

        // draw terrain wireframe
        DrawModelWires(terrain, {0, 0, 0}, 1.0f, Fade(LIGHTGRAY, 0.5f));

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    return 0;
}
