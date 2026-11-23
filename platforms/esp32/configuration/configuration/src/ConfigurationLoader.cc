#include "esp32/configuration/configuration/ConfigurationLoader.hh"

namespace esp32::configuration
{
    std::expected<::configuration::Configuration, ::configuration::interface::ConfigurationLoaderError>
    ConfigurationLoader::loadConfiguration()
    {
        ::configuration::Configuration config;
        config.wifi.ssid = "ESP32_SSID";
        config.wifi.password = "ESP32_PASSWORD";
        return config;
        // return std::unexpected(::configuration::interface::ConfigurationLoaderError::CONFIGURATION_NOT_FOUND);
    }
}