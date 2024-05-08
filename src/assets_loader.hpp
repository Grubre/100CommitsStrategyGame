#pragma once
#include "error.hpp"
#include <expected>
#include <filesystem>
#include <raylib.h>
#include <string>
#include <type_traits>

namespace stratgame {
[[nodiscard]] auto get_asset_path(const std::filesystem::path &resource_path) -> Expected<std::filesystem::path>;

template <typename Func, typename... Args>
    requires std::invocable<Func, Args...>
[[nodiscard]] auto load_asset(Func func, Args &&...args) {
    static_assert((std::is_convertible_v<Args, std::string> && ...),
                  "All arguments must be convertible to std::string");
    return func(unwrap(get_asset_path(std::forward<Args>(args))).string().c_str()...);
}

} // namespace stratgame
