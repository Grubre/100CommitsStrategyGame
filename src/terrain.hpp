#pragma once

#include <SimplexNoise.h>
#include <cstdint>
#include <entt.hpp>
#include <optional>
#include <raylib.h>
#include <span>
#include <vector>

namespace stratgame {
struct Chunk {
    Mesh mesh;
};

struct TerrainGenerator {
  public:
    TerrainGenerator(uint32_t chunk_resolution, uint32_t chunk_size, const Shader &shader)
        : chunk_resolution(chunk_resolution), chunk_size(chunk_size), shader(shader) {}

    auto generate_chunk(float x, float y, const SimplexNoise &noise) -> Chunk;

  private:
    uint32_t chunk_resolution;
    uint32_t chunk_size;
    Shader shader;
};

auto generate_terrain_mesh(uint32_t rows, uint32_t cols, const std::span<const float> heights,
                           float dist_between_vertices = 2.0f) -> Mesh;

struct GeneratedTerrain {
    using Heights = std::vector<float>;
    Model model;
    Heights heights;
};

struct TerrainClick {
    std::optional<Vector2> position;
};

auto generate_terrain_model(uint32_t rows, uint32_t cols) -> GeneratedTerrain;

auto generate_terrain_shader(const Shader &terrain_shader, float height_scale) -> Shader;

// ==================================================================
// entt integration
// ==================================================================
auto register_chunk(entt::registry &registry, const Chunk &chunk) -> entt::entity;
}; // namespace stratgame
