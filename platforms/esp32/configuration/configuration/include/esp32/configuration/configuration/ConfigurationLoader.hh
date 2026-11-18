#pragma once
#include <configuration/configuration/interface/ConfigurationLoader.hh>
#include <expected>

namespace esp32::configuration
{
    class ConfigurationLoader final : public ::configuration::interface::ConfigurationLoader
    {
    public:
        std::expected<::configuration::Configuration, ::configuration::interface::ConfigurationLoaderError>
        loadConfiguration() override;
    };
}