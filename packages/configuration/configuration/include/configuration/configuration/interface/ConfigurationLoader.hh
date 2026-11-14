#pragma once
#include "configuration/configuration/Configuration.hh"
#include <expected>

namespace configuration::interface
{
    enum class ConfigurationLoaderError
    {
        FILE_NOT_FOUND,
        INVALID_FORMAT,
        UNKNOWN_ERROR
    };
    class ConfigurationLoader
    {
    public:
        virtual ~ConfigurationLoader() = default;

        virtual std::expected<configuration::Configuration, ConfigurationLoaderError> loadConfiguration() = 0;
    }
}