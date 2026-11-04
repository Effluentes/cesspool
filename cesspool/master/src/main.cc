#include <composite_root/app/App.hh>
#include <composite_root/system_orchestrator/SystemOrchestrator.hh>
#include <cstdio>
#include <qpcpp.hpp>
#include <array>
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
static const char *TAG = "QP";

namespace QP
{
    void QF::onStartup()
    {
        // ESP_LOGI("QP", "QF started");
    }

    void QF::onCleanup() {}
} // namespace QP

extern "C" Q_NORETURN Q_onAssert(char_t const *const module, int_t location)
{
    // ESP_LOGE(TAG, "ASSERT: %s:%d", module, location);
    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}

extern "C" void app_main()
{
    printf("Starting Cesspool application...\n");
    runtime::composite_root::SystemOrchestrator orchestrator;
    runtime::composite_root::App app(orchestrator);
    app.run();
    QP::QF::run();
}