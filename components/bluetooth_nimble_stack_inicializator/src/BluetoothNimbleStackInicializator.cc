#include "platform/bluetooth_nimble_stack_inicializator/BluetoothNimbleStackInicializator.hh"
// #include "nimble/nimble_port.h"
// #include "nimble/nimble_port_freertos.h"
#include "esp_log.h"

namespace platform
{
    BluetoothNimbleStackInicializator::BluetoothNimbleStackInicializator() {}

    void BluetoothNimbleStackInicializator::initialize() {
        ESP_LOGI("BluetoothNimbleStackInicializator", "Initializing Bluetooth Nimble Stack");
        // nimble_port_init();
        ESP_LOGI("BluetoothNimbleStackInicializator", "Bluetooth Nimble Stack initialized");
    }

    bool BluetoothNimbleStackInicializator::isInitialized() const {
        return true;
    }
}