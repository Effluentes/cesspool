#define BSP_TICKS_PER_SEC 100
#define Q_EVT_POOL   1
#define QF_MAX_POOLS 1
#define QF_EVENT_SIZ sizeof(QP::QEvt)
#define QF_MAX_TICK_RATE 1
#define Q_ASSERT 1

#include "qpcpp.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"               // <-- DODANE

static const char * const TAG = "APP";
#define LED_GPIO GPIO_NUM_19

// Eventy
namespace Blinker {
    enum Signals { TICK_SIG = QP::Q_USER_SIG };
    struct TickEvt : public QP::QEvt {
        TickEvt(QP::QSignal sig) : QP::QEvt{ sig, 0U, 0U } {}
    };
}

// BlinkerAO
namespace Blinker {
    class AO final : public QP::QActive {
    public:
        AO() : QP::QActive(Q_STATE_CAST(&Initial)) {}
    private:
        static QP::QState Initial(AO * const me, QP::QEvt const * const e) {
            switch (e->sig) {
            case Q_ENTRY_SIG:
                gpio_reset_pin(LED_GPIO);
                gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
                gpio_set_level(LED_GPIO, 0);
                ESP_LOGI(TAG, "Blinker: ready");
                return me->tran(Q_STATE_CAST(&Blinking));
            default: return Q_SUPER(&QHsm::top);
            }
        }
        static QP::QState Blinking(AO * const me, QP::QEvt const * const e) {
            switch (e->sig) {
            case Q_ENTRY_SIG:
                ESP_LOGI(TAG, "Blinker: waiting for ticks...");
                return Q_HANDLED();
            case TICK_SIG: {
                static bool led_state = false;
                led_state = !led_state;
                gpio_set_level(LED_GPIO, led_state);
                ESP_LOGI(TAG, "Blinker: LED %s", led_state ? "ON" : "OFF");
                return Q_HANDLED();
            }
            default: return Q_SUPER(&QHsm::top);
            }
        }
    };
    AO BlinkerAO;
    QP::QEvt const *BlinkerQueue[10];
}

// TimerAO
namespace Timer {
    class AO final : public QP::QActive {
    private:
        QP::QTimeEvt m_timeEvt;
    public:
        AO() : QP::QActive(Q_STATE_CAST(&Initial)),
               m_timeEvt(this, Blinker::TICK_SIG, 0U) {}
    private:
        static QP::QState Initial(AO * const me, QP::QEvt const * const e) {
            switch (e->sig) {
            case Q_ENTRY_SIG:
                me->m_timeEvt.armX(50U, 50U);   // 500 ms
                ESP_LOGI(TAG, "Timer: started");
                return me->tran(Q_STATE_CAST(&Running));
            default: return Q_SUPER(&QHsm::top);
            }
        }
        static QP::QState Running(AO * const me, QP::QEvt const * const e) {
            switch (e->sig) {
            case Q_ENTRY_SIG: return Q_HANDLED();
            case Blinker::TICK_SIG: {
                Blinker::TickEvt *tickEvt = Q_NEW(Blinker::TickEvt, Blinker::TICK_SIG);
                ESP_LOGI(TAG, "Timer: tick generated");
                Blinker::BlinkerAO.POST(tickEvt, 0U);
                return Q_HANDLED();
            }
            default: return Q_SUPER(&QHsm::top);
            }
        }
    };
    AO TimerAO;
    QP::QEvt const *TimerQueue[5];
}

// QP callbacks
extern "C" Q_NORETURN Q_onAssert(char_t const * const module, int_t location) {
    ESP_LOGE(TAG, "QP ASSERT: module %s, location %d", module, location);
    while (1);
}
namespace QP {
    void QF::onStartup() { ESP_LOGI(TAG, "QF::onStartup"); }
    void QF::onCleanup() { ESP_LOGI(TAG, "QF::onCleanup"); }
}

// ===== Funkcja wywoływana z timera sprzętowego ESP32 =====
static void qp_tick_callback(void* arg) {
    QP::QF::tickX_(0U);
}

// app_main
extern "C" void app_main() {
    QP::QF::init();

    static QP::QEvt l_poolSto[20];
    QP::QF::poolInit(l_poolSto, sizeof(l_poolSto), sizeof(QP::QEvt));

    // Start obiektów aktywnych
    Timer::TimerAO.start(1U, Timer::TimerQueue, Q_DIM(Timer::TimerQueue),
                         nullptr, 2048U, nullptr);
    Blinker::BlinkerAO.start(2U, Blinker::BlinkerQueue, Q_DIM(Blinker::BlinkerQueue),
                             nullptr, 2048U, nullptr);

    // Timer sprzętowy ESP32 – generuje tick co 10 ms
    const esp_timer_create_args_t timer_args = {
        .callback = &qp_tick_callback,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_TASK,      // wywoływane w przerwaniu
        .name = "qp_tick"
    };
    esp_timer_handle_t qp_timer;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &qp_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(qp_timer, 10000)); // okres 10 ms

    QP::QF::run();
}