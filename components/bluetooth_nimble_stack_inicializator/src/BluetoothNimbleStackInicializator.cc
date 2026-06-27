#include "platform/bluetooth_nimble_stack_inicializator/BluetoothNimbleStackInicializator.hh"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "services/gap/ble_svc_gap.h"
#include "nvs_flash.h"
#include "host/ble_hs.h"

namespace {
    void ble_host_task(void*)
    {
        nimble_port_run();
        nimble_port_freertos_deinit();
    }
}

namespace platform
{
    BluetoothNimbleStackInicializator::BluetoothNimbleStackInicializator() {}

    void BluetoothNimbleStackInicializator::initialize() {
        ESP_LOGI("BluetoothNimbleStackInicializator", "Initializing Bluetooth Nimble Stack");

        esp_err_t err = nvs_flash_init();
        if (err != ESP_OK && err != ESP_ERR_NVS_NO_FREE_PAGES && err != ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_LOGE("BluetoothNimbleStackInicializator", "nvs_flash_init failed: %s", esp_err_to_name(err));
            return;
        }

        err = nimble_port_init();
        if (err != ESP_OK) {
            ESP_LOGE("BluetoothNimbleStackInicializator", "nimble_port_init failed: %s", esp_err_to_name(err));
            return;
        }

        ble_svc_gap_init();
        nimble_port_freertos_init(ble_host_task);
        ESP_LOGI("BluetoothNimbleStackInicializator", "Bluetooth Nimble Stack initialized");
    }

    bool BluetoothNimbleStackInicializator::isInitialized() const {
        return true;
    }
}