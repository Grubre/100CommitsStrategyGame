#include "terrain.hpp"
#include <SimplexNoise.h>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <vector>

namespace stratgame {
auto generate_terrain_mesh(uint32_t rows, uint32_t cols, const std::span<const float> heights,
                           float dist_between_vertices) -> Mesh {
    assert(static_cast<unsigned long>(rows * cols) == heights.size());

    auto mesh = Mesh{};

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

    auto k = 0u;
    for (auto i = 0u; i < rows - 1; i++) {
        for (auto j = 0u; j < cols - 1; j++) {
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

auto generate_terrain_model(uint32_t rows, uint32_t cols) -> Model {
    auto heights = std::vector<float>(rows * cols);

    auto height_scale = 5.f;

    const auto noise = SimplexNoise();

    for (uint32_t i = 0; i < rows; i++) {
        for (uint32_t j = 0; j < cols; j++) {
            const auto index = i * cols + j;
            const auto x = static_cast<float>(j) / static_cast<float>(cols);
            const auto y = static_cast<float>(i) / static_cast<float>(rows);
            heights[index] = noise.fractal(1, x, y) * height_scale;
        }
    }

    auto terrain_mesh = generate_terrain_mesh(rows, cols, heights, 0.5f);

    auto terrain = LoadModelFromMesh(terrain_mesh);

    return terrain;
}

auto generate_terrain_shader(const std::filesystem::path &vert_path, const std::filesystem::path &frag_path,
                             float height_scale) -> Shader {
    if (!std::filesystem::exists(vert_path) || !std::filesystem::exists(frag_path)) {
        std::cerr << "Could not find terrain shader files\n";
        std::exit(1);
    }

    auto terrain_shader = LoadShader(vert_path.c_str(), frag_path.c_str());

    auto yellow_threshold_loc = GetShaderLocation(terrain_shader, "yellow_threshold");
    float yellow_threshold = 0.02f * height_scale;
    SetShaderValue(terrain_shader, yellow_threshold_loc, &yellow_threshold, SHADER_UNIFORM_FLOAT);

    auto white_threshold_loc = GetShaderLocation(terrain_shader, "white_threshold");
    float white_threshold = 0.7f * height_scale;
    SetShaderValue(terrain_shader, white_threshold_loc, &white_threshold, SHADER_UNIFORM_FLOAT);

    return terrain_shader;
}
}; // namespace stratgame
