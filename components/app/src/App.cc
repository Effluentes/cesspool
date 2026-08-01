#include "app/App.hh"

#include <qpcpp.hpp>
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include <platform_driver_esp32/GPIO.hh>
#include <platform_driver_esp32/I2C.hh>
#include <platform_driver_esp32/Sh1106.hh>
#include <platform_driver_esp32/EncoderDriver.hh>
#include <bluetooth_manager_ao/BluetoothManagerAO.hh>
#include <platform/bluetooth_nimble_stack_inicializator/BluetoothNimbleStackInicializator.hh>
#include <blackboard_system_state_ao/SystemStateAO.hh>
#include <screen_manager_ao/ScreenManagerAO.hh>

static const char *TAG = "App";

namespace
{
    Esp32GPIODriver ledRed{GPIO_NUM_3};
    Esp32GPIODriver ledBlue{GPIO_NUM_5};
    Esp32GPIODriver ledGreen{GPIO_NUM_4};
    Esp32GPIODriver tra(GPIO_NUM_6);
    Esp32GPIODriver trb(GPIO_NUM_7);
    Esp32I2CWires i2cWires{tra, trb};
    Esp32I2CDriver i2c{i2cWires};
    Sh1106Driver oled{i2c};

    EncoderDriver encoder{GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_18};

    SystemStateAO sysAO{};
    ScreenManagerAO screenAO{oled};
    BluetoothManagerAO btAO{ledRed, ledBlue, ledGreen};

    QP::QEvt const *sysQueue[16];
    QP::QEvt const *screenQueue[8];
    QP::QEvt const *btQueue[10];

} // namespace

// ─────────────────────────────────────────────────────────────────

App::App() = default;

void App::run()
{
    esp_err_t nvsErr = nvs_flash_init();
    if (nvsErr == ESP_ERR_NVS_NO_FREE_PAGES ||
        nvsErr == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvsErr = nvs_flash_init();
    }
    if (nvsErr != ESP_OK)
        ESP_LOGE(TAG, "nvs_flash_init: %s", esp_err_to_name(nvsErr));

    static QP::QSubscrList subscrSto[MAX_PUB_SIG];
    QP::QF::psInit(subscrSto, Q_DIM(subscrSto));

    static uint8_t evPool[64 * sizeof(SystemStateSnapshotEvt)];
    QP::QF::poolInit(evPool, sizeof(evPool), sizeof(SystemStateSnapshotEvt));

    oled.initialize();
    // encoder.initialize();

    platform::BluetoothNimbleStackInicializator nimbleInit;
    nimbleInit.initialize();

    sysAO.start(5U, sysQueue, Q_DIM(sysQueue), nullptr, 2048U, nullptr);
    // screenAO.start(4U, screenQueue, Q_DIM(screenQueue));
    btAO.start(3U, btQueue, Q_DIM(btQueue),
               nullptr, 2048U, nullptr);

    QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_START_SIG), nullptr);

    QP::QF::run();
}