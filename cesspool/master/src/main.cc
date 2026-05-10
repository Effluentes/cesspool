#define BSP_TICKS_PER_SEC 100

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <cstdlib>

#include "qpcpp.hpp"
#include "platform/bsp/storage/nvs/NonVolatileStorage.hh"

static const char *TAG = "app";

#define LED_GPIO GPIO_NUM_19

enum MySignals {
    TIME_TICK_SIG = QP::Q_USER_SIG,
};

extern "C" Q_NORETURN Q_onAssert(char_t const * const module, int_t location) {
    ESP_LOGE(TAG, "Q_onAssert: module:%s loc:%d\n", module, location);
    while(1);
}

namespace QP {
void QF::onStartup() { ESP_LOGI(TAG, "QP::QF::onStartup()"); }
void QF::onCleanup() { ESP_LOGI(TAG, "QP::QF::onCleanup()"); }
}

// ---------- BlinkerAO ----------
namespace Blinker {

class AO final : public QP::QActive {
public:
    AO() : QP::QActive(Q_STATE_CAST(&AO::initial)) {}

private:
    static QP::QState initial(AO * const me, QP::QEvt const * const) {
        gpio_reset_pin(LED_GPIO);
        gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(LED_GPIO, 0);
        return me->tran(Q_STATE_CAST(&AO::blinking));
    }

    static QP::QState blinking(AO * const me, QP::QEvt const * const e) {
        switch (e->sig) {
        case Q_ENTRY_SIG:
            ESP_LOGI(TAG, "BlinkerAO: gotowy");
            return Q_HANDLED();
        case TIME_TICK_SIG: {
            static bool led_state = false;
            led_state = !led_state;
            gpio_set_level(LED_GPIO, led_state ? 1 : 0);
            ESP_LOGI(TAG, "BlinkerAO: LED %s", led_state ? "ON" : "OFF");
            return Q_HANDLED();
        }
        default:
            return Q_SUPER(&QP::QHsm::top);
        }
    }
};

AO blinkerAO;
QP::QEvt const *blinkerQueue[10];

} // namespace Blinker

// ---------- TimerAO ----------
namespace Timer {

class AO final : public QP::QActive {
private:
    QP::QTimeEvt m_timeEvt;

public:
    AO()
        : QP::QActive(Q_STATE_CAST(&AO::initial)),
          m_timeEvt(this, TIME_TICK_SIG, 0U)
    {}

private:
    static QP::QState initial(AO * const me, QP::QEvt const * const) {
        me->m_timeEvt.armX(100U, 100U);   // 500 ms
        ESP_LOGI(TAG, "TimerAO: timer uzbrojony");
        return me->tran(Q_STATE_CAST(&AO::running));
    }

    static QP::QState running(AO * const me, QP::QEvt const * const e) {
        switch (e->sig) {
        case Q_ENTRY_SIG:
            return Q_HANDLED();
        case TIME_TICK_SIG: {
            static QP::QEvt const tickEvt = { TIME_TICK_SIG, 0U, 0U };
            Blinker::blinkerAO.POST(&tickEvt, 0U);
            return Q_HANDLED();
        }
        default:
            return Q_SUPER(&QP::QHsm::top);
        }
    }
};

AO timerAO;
QP::QEvt const *timerQueue[5];

} // namespace Timer

// ---------- Zadanie tick ----------
namespace {

void tickTask(void * /*param*/) {
    TickType_t lastWake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(10);
    while (true) {
        vTaskDelayUntil(&lastWake, period);
        QP::QF::tickX_(0U);
    }
}

} // namespace

// ---------- app_main ----------
extern "C" void app_main() {
    platform::bsp::NonVolatileStorage storage;
    if (auto result = storage.initialize(); !result) {
        ESP_LOGE("app_main", "NVS init failed: %d", static_cast<int>(result.error()));
    }

    QP::QF::init();

    // Uruchom tick QP (wysoki priorytet)
    static StackType_t tickStack[configMINIMAL_STACK_SIZE];
    static StaticTask_t tickTaskBuf;
    xTaskCreateStaticPinnedToCore(
        tickTask, "qp_tick",
        configMINIMAL_STACK_SIZE, nullptr,
        configMAX_PRIORITIES - 1,
        tickStack, &tickTaskBuf, tskNO_AFFINITY);

    Timer::timerAO.start(1U, Timer::timerQueue,
                         Q_DIM(Timer::timerQueue),
                         nullptr, 2048U, nullptr);

    Blinker::blinkerAO.start(2U, Blinker::blinkerQueue,
                             Q_DIM(Blinker::blinkerQueue),
                             nullptr, 2048U, nullptr);

    QP::QF::run();
}