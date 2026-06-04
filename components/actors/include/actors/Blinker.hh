#include "qpcpp.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
static const char* TAG = "Blinker";
constexpr gpio_num_t LED_GPIO = GPIO_NUM_19;

namespace Blinker {
enum class Signal : uint32_t {
    Tick = QP::Q_USER_SIG,
    // kolejne sygnały...
};
inline constexpr int QUEUE_DEPTH = 10;

class AO final : public QP::QActive {
    QP::QTimeEvt m_timeEvt;
    bool         m_ledState{false};
public:
    AO()
        : QP::QActive{Q_STATE_CAST(&Initial)},
          m_timeEvt{this, static_cast<QP::QSignal>(Signal::Tick), 0U}
    {}

private:
    static QP::QState Initial(AO* me, QP::QEvt const* e) {
        (void)e;
        gpio_reset_pin(LED_GPIO);
        gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(LED_GPIO, 0);
        me->m_timeEvt.armX(50U, 50U);
        return me->tran(Q_STATE_CAST(&Blinking));
    }

    static QP::QState Blinking(AO* me, QP::QEvt const* e) {
        switch (e->sig) {
        case Q_ENTRY_SIG:
            ESP_LOGI(TAG, "running");
            return Q_HANDLED();
        case static_cast<QP::QSignal>(Signal::Tick):
            me->m_ledState = !me->m_ledState;
            gpio_set_level(LED_GPIO, me->m_ledState);
            ESP_LOGI(TAG, "LED %s", me->m_ledState ? "ON" : "OFF");
            return Q_HANDLED();
        default:
            return Q_SUPER(&QHsm::top);
        }
    }
};

// Statyczna alokacja — zero heap
static AO             s_instance;
static QP::QEvt const* s_queue[QUEUE_DEPTH];

} // namespace Blinker