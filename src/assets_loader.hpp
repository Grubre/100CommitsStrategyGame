#pragma once
#include "error.hpp"
#include <expected>
#include <filesystem>
#include <raylib.h>
#include <string>

namespace stratgame {
auto get_asset_path(const std::string &resource_path) -> Expected<std::filesystem::path>;

template <typename T, typename Func, typename... Args> auto load_asset(Func func, Args &&...args) -> T {
    static_assert((std::is_convertible_v<Args, std::string> && ...),
                  "All arguments must be convertible to std::string");
    return func(unwrap(get_asset_path(std::forward<Args>(args))).string().c_str()...);
}

} // namespace stratgame
