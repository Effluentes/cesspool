#include "app/App.hh"
#include <actors/radio_ao/RadioAO.hh>
#include <limits>
#include <platform_driver_esp32/GPIO.hh>
#include <bluetooth_manager_ao/BluetoothManagerAO.hh>
#include <qpcpp.hpp>
#include <platform/bluetooth_nimble_stack_inicializator/BluetoothNimbleStackInicializator.hh>
#include "nvs_flash.h"
#include "esp_log.h"
App::App() {}

namespace {
    // piny LED
    Esp32GPIODriver s_ledRed   { GPIO_NUM_3 };
    Esp32GPIODriver s_ledBlue  { GPIO_NUM_5 };
    Esp32GPIODriver s_ledGreen { GPIO_NUM_4 };

    // kolejka AO — tutaj, nie w headerze
    QP::QEvt const* s_btQueue[10];

    // AO — dostaje referencje do GPIO, nie wie że to ESP32
    BluetoothManagerAO s_btAO { s_ledRed, s_ledBlue, s_ledGreen };
    const uint8_t MAX_PUB_SIG = 10;
    const auto subscrSto = new QP::QSubscrList[MAX_PUB_SIG];
}

void App::run()  {
    esp_err_t nvsErr = nvs_flash_init();
    if (nvsErr != ESP_OK && nvsErr != ESP_ERR_NVS_NO_FREE_PAGES && nvsErr != ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGE("App", "nvs_flash_init failed: %s", esp_err_to_name(nvsErr));
    }

    static QP::QSubscrList subscrSto[MAX_PUB_SIG];
    QP::QF::psInit(subscrSto, Q_DIM(subscrSto));

    static uint8_t evPool[64 * sizeof(BtDataEvt)];
    QP::QF::poolInit(evPool, sizeof(evPool), sizeof(BtDataEvt));

    platform::BluetoothNimbleStackInicializator nimbleInit;
    nimbleInit.initialize();

    s_btAO.start(3U, s_btQueue, Q_DIM(s_btQueue), nullptr, 2048U, nullptr);

    QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_START_SIG), nullptr);

    QP::QF::run();
}