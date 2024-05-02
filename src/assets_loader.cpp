#include <filesystem>
#include <expected>

namespace stratgame {

auto get_asset_path(const std::string& resource_path) -> std::expected<std::filesystem::path, std::string> {
    // If we have a resources directory defined, use that
#ifdef RESOURCES_DIR
    auto full_path = std::filesystem::path(RESOURCES_DIR) / resource_path;
    if (std::filesystem::exists(full_path)) {
        return full_path;
    }
#endif

    // Otherwise, use the relative path
    if (std::filesystem::exists(resource_path)) {
        return resource_path;
    }

    return std::unexpected("Resource not found: " + resource_path);
}

} // namespace stratgame
