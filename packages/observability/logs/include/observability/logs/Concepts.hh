#pragma once

#include <concepts>
#include <cstdint>

#include "observability/logs/LogRecord.hh"

namespace observability::logs
{
    // Wyjście loggera. Hot path: nie blokuje, nie alokuje, nie rzuca.
    template <typename S>
    concept LogSink = requires(S &s, const LogRecord &r) {
        { s.write(r) } noexcept;
    };

    template <typename C>
    concept MonotonicClock = requires(const C &c) {
        { c.nowUs() } noexcept -> std::same_as<std::uint64_t>;
    };

    // Sekcja krytyczna wstrzykiwana z platformy (PRIMASK / portENTER_CRITICAL / mutex).
    template <typename C>
    concept CriticalSection = requires(C &c, std::uint32_t key) {
        { c.enter() } noexcept -> std::same_as<std::uint32_t>;
        { c.exit(key) } noexcept;
    };
} // namespace observability::logs
