#pragma once
#include <expected>
#include <filesystem>
#include <format>
#include <string>
#include <iostream>

namespace stratgame {
auto get_asset_path(const std::string& resource_path) -> std::expected<std::filesystem::path, std::string>;

template<typename T>
auto unwrap(const std::expected<T, std::string>& exp) -> T {
    if (!exp.has_value()) {
        std::cerr << std::format("Error: {}\n", exp.error());
        std::exit(1);
    }

    return exp.value();
}

} // namespace stratgame
