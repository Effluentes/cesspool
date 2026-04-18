#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <cstdlib>

#include "qpcpp.hpp"
#include "platform/bsp/storage/nvs/NonVolatileStorage.hh"

static const char *TAG = "app";

// Wymagane przez QP (qassert.h): wywoływane przy Q_ASSERT / Q_REQUIRE wewnątrz frameworka.
extern "C" Q_NORETURN Q_onAssert(char_t const * const module, int_t location)
{
    ESP_LOGE(TAG, "Q_onAssert: module:%s loc:%d\n", module, location);
    while(1);

}

namespace QP {
// Wywoływane przez port z qf_port.cpp przy starcie frameworka (np. konfiguracja timerów QP).
void QF::onStartup() { ESP_LOGI(TAG, "QP::QF::onStartup()"); }

// Wywoływane przy kończeniu (np. sprzątanie zasobów).
void QF::onCleanup() { ESP_LOGI(TAG, "QP::QF::onCleanup()"); }
}  // namespace QP

namespace {
// Minimalny Active Object: jeden stan, log przy wejściu (hello world).
class HelloWorld final : public QP::QActive {
public:
    HelloWorld()
        : QP::QActive(Q_STATE_CAST(&HelloWorld::initial)) {}

private:
    static QP::QState initial(HelloWorld * const me, QP::QEvt const * const) {
        return me->tran(Q_STATE_CAST(&HelloWorld::active));
    }

    static QP::QState active(HelloWorld * const me, QP::QEvt const * const e) {
        switch (e->sig) {
        case Q_ENTRY_SIG:
            ESP_LOGI(TAG, "Hello World z QP/C++ (Q_ENTRY_SIG w stanie active)");
            return Q_HANDLED();
        default:
            return Q_SUPER(&QP::QHsm::top);
        }
    }
};

HelloWorld helloAO;
QP::QEvt const *helloQueue[10];
}  // namespace

extern "C" void app_main() {
    platform::bsp::NonVolatileStorage storage;
    auto const result = storage.initialize();
    if (!result) {
        ESP_LOGE("app_main", "Nie udało się zainicjować storage: %d",
                 static_cast<int>(result.error()));
        return;
    }

    QP::QF::init();

    // Priorytet 1, własna kolejka eventów — standardowy start AO.
    helloAO.start(
        1U,
        helloQueue,
        Q_DIM(helloQueue),
        nullptr,
        2048U, //wieksze niz 0, bo w portach FreeRTOS taski muszą mieć stos (w przeciwieństwie do portów na systemy operacyjne, gdzie stos jest współdzielony).
        nullptr);

    // Blokuje: pętla dispatch eventów QP (FreeRTOS task wewnątrz portu).
    QP::QF::run();
}
