#pragma once
#include "configuration/configuration/Configuration.hh"
#include <expected>

namespace esp32::configuration
{
    class ConfigurationLoader
    {
    public:
        virtual ~ConfigurationLoader() = default;

        virtual std::expected<configuration::Configuration, ConfigurationLoaderError> loadConfiguration() override = 0;
    }
}