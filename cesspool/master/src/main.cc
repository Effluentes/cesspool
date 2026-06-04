#include "qpcpp.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"

static const char * const TAG = "APP";
constexpr gpio_num_t LED_GPIO = GPIO_NUM_19;

// ---------------------------------------------------------------------------
namespace Blinker {

enum Signals : QP::QSignal { TICK_SIG = QP::Q_USER_SIG };

class AO final : public QP::QActive {
    QP::QTimeEvt m_timeEvt;
    bool         m_ledState{false};
public:
    AO()
      : QP::QActive{Q_STATE_CAST(&Initial)},
        m_timeEvt{this, TICK_SIG, 0U}
    {}

private:
    static QP::QState Initial(AO * const me, QP::QEvt const * const e) {
        (void)e;
        gpio_reset_pin(LED_GPIO);
        gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(LED_GPIO, 0);
        me->m_timeEvt.armX(50U, 50U);   // 50 ticks * 10ms = 500ms
        return me->tran(Q_STATE_CAST(&Blinking));
    }

    static QP::QState Blinking(AO * const me, QP::QEvt const * const e) {
        switch (e->sig) {
        case Q_ENTRY_SIG:
            ESP_LOGI(TAG, "Blinker: running");
            return Q_HANDLED();
        case TICK_SIG:
            me->m_ledState = !me->m_ledState;
            gpio_set_level(LED_GPIO, me->m_ledState);
            ESP_LOGI(TAG, "LED %s", me->m_ledState ? "ON" : "OFF");
            return Q_HANDLED();
        default:
            return Q_SUPER(&QHsm::top);
        }
    }
};

static AO             instance;
static QP::QEvt const *queue[10];

} // namespace Blinker

// ---------------------------------------------------------------------------
extern "C" Q_NORETURN Q_onAssert(char_t const * const module, int_t location) {
    ESP_LOGE(TAG, "ASSERT: %s:%d", module, location);
    taskDISABLE_INTERRUPTS();
    for (;;) {}
}

namespace QP {
    void QF::onStartup() { ESP_LOGI(TAG, "QF started"); }
    void QF::onCleanup() {}
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

// ---------------------------------------------------------------------------
extern "C" void app_main() {
    QP::QF::init();

    alignas(QP::QEvt) static uint8_t poolSto[20 * sizeof(QP::QEvt)];
    QP::QF::poolInit(poolSto, sizeof(poolSto), sizeof(QP::QEvt));

    Blinker::instance.start(
        1U,
        Blinker::queue, Q_DIM(Blinker::queue),
        nullptr, 2048U,
        nullptr
    );

    const esp_timer_create_args_t timerArgs = {
        .callback             = &qp_tick_cb,
        .arg                  = nullptr,
        .dispatch_method      = ESP_TIMER_TASK,
        .name                 = "qp_tick",
        .skip_unhandled_events = true,
    };
    esp_timer_handle_t timer;
    ESP_ERROR_CHECK(esp_timer_create(&timerArgs, &timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, 10'000)); // 10 ms

    QP::QF::run();
}