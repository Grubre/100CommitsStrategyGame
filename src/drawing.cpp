#include "drawing.hpp"
#include "camera.hpp"
#include "common_components.hpp"
#include <functional>
#include <print>
#include <raymath.h>

namespace stratgame {
void draw_models(const entt::registry &registry) {
    const auto view = registry.view<ModelComponent, stratgame::Transform>();
    for (auto entity : view) {
        const auto &model_component = view.get<ModelComponent>(entity);

        if (!model_component.visible) {
            continue;
        }

        const auto &transform = view.get<stratgame::Transform>(entity);

        if (registry.any_of<ShaderComponent>(entity)) {
            const auto &shader_component = registry.get<ShaderComponent>(entity);
            const auto shader_backup = model_component.model.materials[0].shader;
            model_component.model.materials[0].shader = shader_component.shader;
            DrawModel(model_component.model, transform.position, 1.0f, WHITE);
            model_component.model.materials[0].shader = shader_backup;
            continue;
        }
        DrawModel(model_component.model, transform.position, model_component.scale, WHITE);
    }
}

void flag_culled_models(entt::registry &registry) {
    constexpr auto half_pi = std::numbers::pi_v<float> / 2.f;

    const auto models_view = registry.view<ModelComponent, stratgame::Transform, FrustumCullingComponent>();
    const auto camera_entity = registry.view<stratgame::Camera>().front();
    const auto &camera = registry.get<stratgame::Camera>(camera_entity);
    const auto camera_pos = camera.get_source_position();
    const auto fovx = 180.f;
    const auto alpha = (180 - fovx) / 2;
    const auto sin_alpha = sin(alpha);
    const auto cos_alpha = cos(alpha);
    const auto tan_alpha = tan(alpha);

    for (auto model_entity : models_view) {
        auto &model_component = models_view.get<ModelComponent>(model_entity);
        const auto &transform = models_view.get<stratgame::Transform>(model_entity);
        const auto &frustum_culling_component = models_view.get<FrustumCullingComponent>(model_entity);

        const auto culled_center = frustum_culling_component.get_sphere_center(transform.position);
        DrawSphere(culled_center, 1.f, RED);

        const auto check_in_frustum = [&](bool to_right) {
            const auto coeffx = to_right ? -1.f : 1.f;
            const auto coeffy = 1.f;
            const auto coefff = to_right ? 1.f : -1.f;

            const auto frustum_sphere_center = frustum_culling_component.get_sphere_center(transform.position);
            const auto normalized_model_transform_3d = Vector3Subtract(frustum_sphere_center, camera_pos);
            const auto normalized_model_transform_2d = Vector2(normalized_model_transform_3d.x, normalized_model_transform_3d.z);
            const auto rotated_model_transform = Vector2Rotate(normalized_model_transform_2d, camera.yaw + half_pi);

            const auto sphere_x = rotated_model_transform.x + coeffx * frustum_culling_component.radius * sin_alpha;
            const auto sphere_z = rotated_model_transform.y + coeffy * frustum_culling_component.radius * cos_alpha;

            return sphere_z <= coefff * tan_alpha * sphere_x;
        };

        model_component.visible = check_in_frustum(true) && check_in_frustum(false);
    }
}

auto register_instanceable_model(entt::registry &registry, const Model &model) -> entt::entity {
    static int model_id = 0;

    std::println("Registering model with id: {}", model_id);

    const auto entity = registry.create();
    registry.emplace<InstanceableModel>(entity, model_id, model, std::vector<Matrix>{});

    model_id++;
    return entity;
}

void create_model_instance(entt::registry &registry, entt::entity model_entity, Vector3 transform,
                           entt::entity object_entity) {
    registry.emplace<stratgame::Transform>(object_entity, transform);

    auto &instanceable_model = registry.get<InstanceableModel>(model_entity);
    auto &transforms = instanceable_model.transforms;

    transforms.push_back(MatrixIdentity());
    registry.emplace<ModelInstance>(object_entity, instanceable_model.model_id, static_cast<int>(transforms.size()));
}

void draw_models_instanced(entt::registry &registry) {
    const auto models = registry.view<InstanceableModel>();
    const auto instanced_entities = registry.view<ModelInstance, stratgame::Transform>();

    for (auto model_entity : models) {
        auto &instanceable_model = models.get<InstanceableModel>(model_entity);
        auto &transforms = instanceable_model.transforms;

        for (auto instanced_entity : instanced_entities) {
            const auto &model_instance = instanced_entities.get<ModelInstance>(instanced_entity);
            if (model_instance.model_id != instanceable_model.model_id) {
                continue;
            }

            auto &transform = instanced_entities.get<stratgame::Transform>(instanced_entity);
            transforms[static_cast<std::size_t>(model_instance.instance_id - 1)] =
                MatrixTranslate(transform.position.x, transform.position.y, transform.position.z);
        }

        for (auto i = 0; i < instanceable_model.model.meshCount; i++) {
            DrawMeshInstanced(instanceable_model.model.meshes[i], instanceable_model.model.materials[0],
                              transforms.data(), static_cast<int>(transforms.size()));
        }
    }
}

void draw_model_wireframes(const entt::registry &registry) {
    const auto view = registry.view<ModelComponent, stratgame::Transform, DrawModelWireframeComponent>();
    for (auto entity : view) {
        const auto &model_component = view.get<ModelComponent>(entity);
        const auto &transform = view.get<stratgame::Transform>(entity);
        DrawModelWires(model_component.model, transform.position, 1.0f, Fade(LIGHTGRAY, 0.6f));
    }
}
}; // namespace stratgame
