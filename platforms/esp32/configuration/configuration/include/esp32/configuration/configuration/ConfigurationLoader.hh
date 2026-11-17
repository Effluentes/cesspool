#pragma once

#include <configuration/configuration/interface/ConfigurationLoader.hh>

namespace esp32::configuration
{
    using ConfigurationLoaderError = ::configuration::interface::ConfigurationLoaderError;

    class ConfigurationLoader final
    {
    public:
        std::expected<::configuration::Configuration, ConfigurationLoaderError>
        loadConfiguration();
    };
}
