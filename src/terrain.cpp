#include "terrain.hpp"
#include "common_components.hpp"
#include "drawing.hpp"
#include <SimplexNoise.h>
#include <numbers>
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
    const auto offset = Vector2{radius / 2.f, radius / 2.f};
    registry.emplace<stratgame::FrustumCullingComponent>(
        entity, stratgame::FrustumCullingComponent{.radius = radius, .offset = offset});

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

}; // namespace stratgame
