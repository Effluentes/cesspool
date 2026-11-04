#pragma once
#include <string>
#include <cstdint>

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
    class Logger
    {
    public:
        virtual ~Logger() = default;
        virtual Logger &getLogger(const std::string &name) = 0;
        virtual void log(const std::string &message, const Severity severity) = 0;
        virtual void trace(const std::string &message) = 0;
        virtual void debug(const std::string &message) = 0;
        virtual void info(const std::string &message) = 0;
        virtual void warning(const std::string &message) = 0;
        virtual void error(const std::string &message) = 0;
        virtual void critical(const std::string &message) = 0;
    };
}