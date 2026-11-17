#include "esp32/configuration/configuration/ConfigurationLoader.hh"

#include <nvs.h>
#include <nvs_flash.h>

namespace esp32::configuration
{
    namespace
    {
        struct NvsHandle
        {
            nvs_handle_t value{0};
            bool opened{false};

            ~NvsHandle()
            {
                if (opened)
                {
                    nvs_close(value);
                }
            }
        };
    }

    std::expected<::configuration::Configuration, ConfigurationLoaderError>
    ConfigurationLoader::loadConfiguration()
    {
        if (nvs_flash_init() != ESP_OK)
        {
            return std::unexpected(ConfigurationLoaderError::UNKNOWN_ERROR);
        }

        NvsHandle handle;
        const auto openResult = nvs_open("cesspool", NVS_READONLY, &handle.value);
        if (openResult == ESP_ERR_NVS_NOT_FOUND)
        {
            return std::unexpected(ConfigurationLoaderError::CONFIGURATION_NOT_FOUND);
        }
        if (openResult != ESP_OK)
        {
            return std::unexpected(ConfigurationLoaderError::UNKNOWN_ERROR);
        }
        handle.opened = true;

        std::size_t ssidSize = 0;
        const auto ssidLookup = nvs_get_str(handle.value, "wifi_ssid", nullptr, &ssidSize);
        if (ssidLookup == ESP_ERR_NVS_NOT_FOUND)
        {
            return std::unexpected(ConfigurationLoaderError::CONFIGURATION_NOT_FOUND);
        }
        if (ssidLookup != ESP_OK || ssidSize < 2 || ssidSize > 33)
        {
            return std::unexpected(ConfigurationLoaderError::INVALID_FORMAT);
        }

        std::size_t passwordSize = 0;
        const auto passwordLookup =
            nvs_get_str(handle.value, "wifi_password", nullptr, &passwordSize);
        if (passwordLookup != ESP_OK || passwordSize == 0 || passwordSize > 65)
        {
            return std::unexpected(ConfigurationLoaderError::INVALID_FORMAT);
        }

        ::configuration::Configuration config;
        if (ssidSize > config.wifi.ssid.capacity() ||
            passwordSize > config.wifi.password.capacity())
        {
            return std::unexpected(ConfigurationLoaderError::INVALID_FORMAT);
        }

        if (nvs_get_str(handle.value, "wifi_ssid", config.wifi.ssid.data(), &ssidSize) != ESP_OK ||
            nvs_get_str(handle.value, "wifi_password", config.wifi.password.data(), &passwordSize) != ESP_OK)
        {
            return std::unexpected(ConfigurationLoaderError::UNKNOWN_ERROR);
        }

        if (!config.wifi.ssid.setSize(ssidSize - 1) ||
            !config.wifi.password.setSize(passwordSize - 1))
        {
            return std::unexpected(ConfigurationLoaderError::INVALID_FORMAT);
        }
        return config;
    }
}
