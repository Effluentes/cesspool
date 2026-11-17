#pragma once

#include <concepts>
#include <cstdint>
#include <string_view>

namespace observability
{
    enum class Severity : std::uint8_t
    {
        INFO = 0,
        TRACE,
        DEBUG,
        WARNING,
        ERROR,
        CRITICAL
    };

    template <typename T>
    concept LoggerPort = requires(T &logger, std::string_view message, Severity severity)
    {
        { logger.log(message, severity) } noexcept -> std::same_as<void>;
    };
}
