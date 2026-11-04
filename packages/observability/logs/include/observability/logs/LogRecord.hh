#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

#include "observability/logs/Format.hh"
#include "observability/logs/Severity.hh"

namespace observability::logs
{
    struct LogArg
    {
        enum class Type : std::uint8_t { None, I32, U32, F32 };
        Type type{Type::None};
        union Value
        {
            std::int32_t i;
            std::uint32_t u;
            float f;
        } v{};
    };

    template <LogArgType T>
    [[nodiscard]] constexpr LogArg makeArg(T x) noexcept
    {
        LogArg a{};
        if constexpr (std::is_floating_point_v<T>)
        {
            a.type = LogArg::Type::F32;
            a.v.f = static_cast<float>(x);
        }
        else if constexpr (std::is_signed_v<T>)
        {
            a.type = LogArg::Type::I32;
            a.v.i = static_cast<std::int32_t>(x);
        }
        else
        {
            a.type = LogArg::Type::U32; // także bool
            a.v.u = static_cast<std::uint32_t>(x);
        }
        return a;
    }

    // Odpowiednik pól trace_id / span_id / trace_flags z OTel LogRecord.
    struct TraceContext
    {
        std::array<std::uint8_t, 16> traceId{};
        std::array<std::uint8_t, 8> spanId{};
        std::uint8_t flags{};
    };

    // Wartość przechodząca przez system. Ciało (body) odtwarza gateway:
    // body = szablon(token) + args; atrybuty = scope (moduł) + resource.
    struct LogRecord
    {
        std::uint64_t timestampUs{}; // zegar monotoniczny, stempluje sink
        std::uint32_t token{};
        Severity severity{Severity::Info};
        std::uint8_t moduleId{};
        std::uint8_t argCount{};
        std::array<LogArg, kMaxArgs> args{};
        TraceContext trace{};
    };

    static_assert(std::is_trivially_copyable_v<LogRecord>);
    static_assert(sizeof(LogRecord) <= 96, "LogRecord ma się mieścić w kilku liniach cache / buforze");
} // namespace observability::logs
