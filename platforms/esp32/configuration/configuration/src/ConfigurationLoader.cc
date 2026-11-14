#include "esp32/configuration/configuration/ConfigurationLoader.hh"

namespace esp32::configuration
{
    std::expected<configuration::Configuration, ConfigurationLoaderError> ConfigurationLoader::loadConfiguration()
    {
        configuration::Configuration config;
        config.wifi.ssid = "ESP32_SSID";
        config.wifi.password = "ESP32_PASSWORD";
        return std::expected<configuration::Configuration, ConfigurationLoaderError>(config);
    }
}