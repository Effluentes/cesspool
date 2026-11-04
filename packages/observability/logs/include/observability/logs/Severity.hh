#pragma once

#include <cstdint>

namespace observability::logs
{
    // Wartości liczbowe = OpenTelemetry SeverityNumber (Logs Data Model).
    // Dzięki temu mapowanie na OTLP jest rzutowaniem, a nie tabelą.
    enum class Severity : std::uint8_t
    {
        Trace = 1,
        Debug = 5,
        Info = 9,
        Warn = 13,
        Error = 17,
        Fatal = 21,
    };

    [[nodiscard]] constexpr std::uint8_t otelSeverityNumber(Severity s) noexcept
    {
        return static_cast<std::uint8_t>(s);
    }

    [[nodiscard]] constexpr const char *otelSeverityText(Severity s) noexcept
    {
        switch (s)
        {
        case Severity::Trace: return "TRACE";
        case Severity::Debug: return "DEBUG";
        case Severity::Info:  return "INFO";
        case Severity::Warn:  return "WARN";
        case Severity::Error: return "ERROR";
        case Severity::Fatal: return "FATAL";
        }
        return "UNSPECIFIED";
    }
} // namespace observability::logs
