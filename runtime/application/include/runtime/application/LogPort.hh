#pragma once

#include <concepts>
#include <string_view>

namespace runtime::application
{
    enum class LogLevel
    {
        info,
        warning,
        error
    };

    template <typename T>
    concept LogWriter = requires(T &writer, LogLevel level, std::string_view message)
    {
        { writer.write(level, message) } noexcept -> std::same_as<void>;
    };

    struct LogPort
    {
        void *context;
        void (*writeFunction)(void *, LogLevel, std::string_view) noexcept;

        template <LogWriter Writer>
        [[nodiscard]] static LogPort bind(Writer &writer) noexcept
        {
            return {
                &writer,
                [](void *context, LogLevel level, std::string_view message) noexcept
                {
                    static_cast<Writer *>(context)->write(level, message);
                }};
        }

        void write(LogLevel level, std::string_view message) const noexcept
        {
            writeFunction(context, level, message);
        }
    };
}
