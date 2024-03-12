#include "SimplexNoise.h"
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

auto generate_terrain_mesh(uint32_t rows, uint32_t cols, const std::span<const float> heights,
                           float dist_between_vertices = 2.0f) -> Mesh {
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
    const int screen_width = 1920;
    const int screen_height = 1080;
    const int fps = 60;

    entt::registry registry;

    const auto camera_entity = stratgame::create_camera(registry);

    InitWindow(screen_width, screen_height, "Hello World!");
    SetTargetFPS(fps);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    constexpr auto rows = 100u;
    constexpr auto cols = 100u;
    auto heights = std::array<float, rows * cols>{};

    const auto noise = SimplexNoise();

    for (uint32_t i = 0; i < rows; i++) {
        for (uint32_t j = 0; j < cols; j++) {
            const auto index = i * cols + j;
            const auto x = static_cast<float>(j) / static_cast<float>(cols);
            const auto y = static_cast<float>(i) / static_cast<float>(rows);
            heights[index] = noise.fractal(10, x, y);
            std::cout << heights[index] << std::endl;
        }
    }

    auto terrain_mesh = generate_terrain_mesh(rows, cols, heights, 0.5f);

    auto terrain = LoadModelFromMesh(terrain_mesh);

    auto terrain_shader = LoadShader("../resources/shaders/terrain.vert", "../resources/shaders/terrain.frag");

    while (!WindowShouldClose()) {
        handle_input(registry);

        update_movement(registry);

        stratgame::update_camera(registry);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        const auto &camera = registry.get<stratgame::Camera>(camera_entity);
        BeginMode3D(camera.camera3d);

        auto shader_backup = terrain.materials[0].shader;
        terrain.materials[0].shader = terrain_shader;
        DrawModel(terrain, {0, 0, 0}, 1.0f, WHITE);
        terrain.materials[0].shader = shader_backup;

        // draw terrain wireframe
        DrawModelWires(terrain, {0, 0, 0}, 1.0f, Fade(LIGHTGRAY, 0.6f));

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadModel(terrain);
    CloseWindow();

    return 0;
}
