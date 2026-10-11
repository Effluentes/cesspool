#pragma once

#include "domain/Errors.hh"

#include <concepts>
#include <expected>
#include <string_view>

template <typename T>
concept Display = requires(T &display, std::string_view text)
{
    { display.draw(text) } -> std::same_as<std::expected<void, cesspool::domain::DisplayError>>;
};
