#include "esp_log.h"
#include "esp_timer.h"
// #include "actors/Blinker.hh"
#include "actors/bluetoothManagerAO/BluetoothManagerAO.hh"
#include "esp_nimble_hci.h"


// ---------------------------------------------------------------------------
namespace QP {
static const char* TAG = "QP";
void QP::QF::onStartup()
{
    ESP_LOGI("QP", "QF started");

}    void QF::onCleanup() {}
}

// ---------------------------------------------------------------------------
// Tick z ESP timer taska — QF::tickX_ jest thread-safe w FreeRTOS porcie
static void qp_tick_cb(void*) {
    QP::QF::tickX_(0U);
    // Gdybyś chciał prawdziwy ISR z ESP_TIMER_ISR (IDF >= 4.3 + sdkconfig):
    //   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    //   QP::QF::TICK_X_FROM_ISR(0U, &xHigherPriorityTaskWoken, nullptr);
    //   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

extern "C" void app_main() {
    QP::QF::init();

    // Pool
    static uint8_t pool[500 * sizeof(QP::QEvt)];
    QP::QF::poolInit(pool, sizeof(pool), sizeof(QP::QEvt));


    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Tick
    static auto tick_cb = [](void*) { QP::QF::tickX_(0U); };
    esp_timer_handle_t timer;
    esp_timer_create_args_t args = {
        .callback = tick_cb,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "tick"
    };
    esp_timer_create(&args, &timer);
    esp_timer_start_periodic(timer, 10'000);

    static QP::QSubscrList subscrSto[BT_MAX_SIG];
    QP::QF::psInit(subscrSto, Q_DIM(subscrSto));
    BluetoothManagerAO::getInstance().start(1U, bluetoothQueue, 10, nullptr, 2048U, nullptr);
    QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_START_SIG), nullptr);
    QP::QF::run();
}