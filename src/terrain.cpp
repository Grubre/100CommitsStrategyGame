#include "terrain.hpp"
#include "common_components.hpp"
#include "drawing.hpp"
#include <SimplexNoise.h>
#include <numbers>
#include <print>
#include <raymath.h>

namespace stratgame {
auto TerrainGenerator::generate_chunk(const std::int64_t x, const std::int64_t y) const -> Chunk {
    const auto transform = Vector3{static_cast<float>(x * chunk_size), 0.f, static_cast<float>(y * chunk_size)};
    auto mesh = generate_flat_chunk_mesh();

    Model model = LoadModelFromMesh(mesh);

    return Chunk{.model = model, .transform = transform};
}

auto TerrainGenerator::register_chunk(entt::registry &registry, const Chunk &chunk) const -> entt::entity {
    const auto entity = registry.create();

    registry.emplace<stratgame::ModelComponent>(entity, chunk.model);
    registry.emplace<stratgame::Transform>(entity, chunk.transform);
    registry.emplace<stratgame::ShaderComponent>(entity, shader);
    registry.emplace<stratgame::DrawModelWireframeComponent>(entity);

    const auto radius = static_cast<float>(chunk_size) * std::numbers::sqrt2_v<float> / 2.f;
    const auto offset = Vector2{static_cast<float>(chunk_size) / 2.f, static_cast<float>(chunk_size) / 2.f};
    registry.emplace<stratgame::FrustumCullingComponent>(
        entity, stratgame::FrustumCullingComponent{.radius = radius, .offset = offset});

    std::println("Registered chunk at ({}, {})", chunk.transform.x, chunk.transform.z);

    return entity;
}

auto TerrainGenerator::generate_flat_chunk_mesh() const -> Mesh {
    Mesh mesh{};

    const auto num_vertices_per_side = chunk_subdivions + 1u;

    mesh.triangleCount = static_cast<int>((num_vertices_per_side - 1u) * (num_vertices_per_side - 1u) * 2u);
    mesh.vertexCount = static_cast<int>(num_vertices_per_side * num_vertices_per_side);
    mesh.vertices = static_cast<float *>(MemAlloc(static_cast<unsigned int>(mesh.vertexCount * 3) * sizeof(float)));
    mesh.indices = static_cast<unsigned short *>(
        MemAlloc(static_cast<unsigned int>(mesh.triangleCount * 3) * sizeof(unsigned short)));

    for (auto i = 0lu; i < num_vertices_per_side; i++) {
        for (auto j = 0lu; j < num_vertices_per_side; j++) {
            const auto index = i * num_vertices_per_side + j;
            mesh.vertices[index * 3] = static_cast<float>(j) * dist_between_vertices();
            mesh.vertices[index * 3 + 1] = 0.f;
            mesh.vertices[index * 3 + 2] = static_cast<float>(i) * dist_between_vertices();
        }
    }

    auto k = 0u;

    for (auto i = 0u; i < num_vertices_per_side - 1; i++) {
        for (auto j = 0u; j < num_vertices_per_side - 1; j++) {
            mesh.indices[k] = static_cast<unsigned short>(i * num_vertices_per_side + j);
            mesh.indices[k + 1] = static_cast<unsigned short>((i + 1) * num_vertices_per_side + j);
            mesh.indices[k + 2] = static_cast<unsigned short>((i + 1) * num_vertices_per_side + (j + 1));
            mesh.indices[k + 3] = static_cast<unsigned short>(i * num_vertices_per_side + j);
            mesh.indices[k + 4] = static_cast<unsigned short>((i + 1) * num_vertices_per_side + (j + 1));
            mesh.indices[k + 5] = static_cast<unsigned short>(i * num_vertices_per_side + (j + 1));
            k += 6; // next quad
        }
    }

    UploadMesh(&mesh, false);

    return mesh;
}

auto generate_terrain_shader(const Shader &terrain_shader, const float height_scale) -> Shader {
    const auto yellow_threshold_loc = GetShaderLocation(terrain_shader, "yellow_threshold");
    const float yellow_threshold = 0.02f * height_scale;
    SetShaderValue(terrain_shader, yellow_threshold_loc, &yellow_threshold, SHADER_UNIFORM_FLOAT);

    const auto white_threshold_loc = GetShaderLocation(terrain_shader, "white_threshold");
    const float white_threshold = 0.7f * height_scale;
    SetShaderValue(terrain_shader, white_threshold_loc, &white_threshold, SHADER_UNIFORM_FLOAT);

    return terrain_shader;
}

[[nodiscard]] auto generate_terrain(entt::registry& registry, const uint32_t size, const int32_t chunk_half_subdivisions, SimplexNoise noise, Shader terrain_shader) -> TerrainGenerator {
    const auto subdivisions = static_cast<uint32_t>(chunk_half_subdivisions * 2);
    const auto chunk_size = size / subdivisions;
    const auto terrain_generator = TerrainGenerator(noise, subdivisions, chunk_size, terrain_shader);

    for(auto x = -chunk_half_subdivisions; x < chunk_half_subdivisions; x++) {
        for(auto y = -chunk_half_subdivisions; y < chunk_half_subdivisions; y++) {
            auto chunk = terrain_generator.generate_chunk(x, y);
            auto chunk_entity = terrain_generator.register_chunk(registry, chunk);
        }
    }

    return terrain_generator;
}

}; // namespace stratgame
