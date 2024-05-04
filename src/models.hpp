#pragma once
#include <expected>
#include <filesystem>
#include <span>
#include <string>
#include <vector>
namespace raylib {
#include "raylib.h"
} // namespace raylib

namespace stratgame::modelmanager {

struct Model {
    raylib::Model model;
    std::string name;
};

struct ModelManager {
    auto load_model(const std::filesystem::path &path) -> std::expected<void, std::string>;
    auto get_models() -> const std::span<const Model>;

  private:
    std::vector<Model> models;
};

} // namespace stratgame::modelmanager
