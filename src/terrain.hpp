#pragma once

#include <SimplexNoise.h>
#include <cstdint>
#include <entt.hpp>
#include <optional>
#include <raylib.h>
#include <vector>

namespace stratgame {
struct Chunk {
    Model model;
    Vector3 transform;
};

struct TerrainGenerator {
  public:
    TerrainGenerator(SimplexNoise noise, uint32_t chunk_resolution, uint32_t chunk_size, const Shader &shader)
        : noise(noise), shader(shader), chunk_size(chunk_size), chunk_subdivions(chunk_resolution) {}

    [[nodiscard]] auto generate_chunk(const std::int64_t x, const std::int64_t y) const -> Chunk;
    auto register_chunk(entt::registry &registry, const Chunk &chunk) const -> entt::entity;

    [[nodiscard]] auto get_noise() const -> const SimplexNoise & { return noise; }

  private:
    SimplexNoise noise;
    Shader shader;

    uint32_t chunk_size;       /// size of the chunk in world units
    uint32_t chunk_subdivions;

    [[nodiscard]] constexpr auto dist_between_vertices() const -> float {
        return static_cast<float>(chunk_size) / static_cast<float>(chunk_subdivions);
    }

    [[nodiscard]] auto generate_flat_chunk_mesh() const -> Mesh;
};

struct GeneratedTerrain {
    using Heights = std::vector<float>;
    Model model;
    Heights heights;
};

struct TerrainClick {
    std::optional<Vector2> position;
};

[[nodiscard]] auto generate_terrain_shader(const Shader &terrain_shader, float height_scale) -> Shader;

}; // namespace stratgame
