#pragma once
#include <expected>
#include <string>
#include <print>

namespace stratgame {
using Error = std::string;
template <typename T> using Expected = std::expected<T, Error>;

[[noreturn]] inline void throw_error(const Error &error) {
    std::println("Error: {}", error);
    std::exit(1);
}

template <typename T> auto unwrap(const Expected<T> &exp) -> T {
    if (!exp.has_value()) {
        throw_error(exp.error());
    }

    return exp.value();
}

} // namespace stratgame
