#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
// qpcpp dopiero po ESP-IDF headers
#include "qpcpp.hpp"
#include "platform/bsp/storage/nvs/NonVolatileStorage.hh"

extern "C" void app_main() {
    platform::bsp::NonVolatileStorage storage;
    auto result = storage.initialize();
    if (!result) {
        ESP_LOGE("app_main", "Failed to initialize storage: %d", static_cast<int>(result.error()));
        return;
    }
    QP::QF::run();
}
