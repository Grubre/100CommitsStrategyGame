#pragma once

#include <cstdint>
#include <filesystem>
#include <raylib.h>
#include <span>
#include <vector>

namespace stratgame {
auto generate_terrain_mesh(uint32_t rows, uint32_t cols, const std::span<const float> heights,
                           float dist_between_vertices = 2.0f) -> Mesh;

struct GeneratedTerrain {
    using Heights = std::vector<float>;
    Model model;
    Heights heights;
};
auto generate_terrain_model(uint32_t rows, uint32_t cols) -> GeneratedTerrain;

auto generate_terrain_shader(const std::filesystem::path &vert_path, const std::filesystem::path &frag_path,
                             float height_scale) -> Shader;

}; // namespace stratgame
