#include "composite_root/App.hh"

#include <platform_driver_esp32/Traits.hh>

#include "esp_log.h"
#include "nvs_flash.h"

namespace
{
constexpr char TAG[] = "App";
}

extern "C" void initializeAppPlatform()
{
    esp_err_t nvsError = nvs_flash_init();
    if (nvsError == ESP_ERR_NVS_NO_FREE_PAGES ||
        nvsError == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvsError = nvs_flash_init();
    }
    if (nvsError != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init failed: %s", esp_err_to_name(nvsError));
    }
}
