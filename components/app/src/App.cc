#include "app/App.hh"
#include <actors/radio_ao/RadioAO.hh>
#include <limits>
#include <platform_driver_esp32/GPIO.hh>

App::App() {}
using Prio = uint8_t;

void App::run() {
    auto ledPin = Esp32GPIODriver(GPIO_NUM_2);
    ledPin.setPinHigh();
    // RadioAO radioAO;
    // Prio priorityForRadioAO = std::numeric_limits<Prio>::max();
    // radioAO.start(priorityForRadioAO);

    // ESP_LOGI("APP", "Starting application");
    // platform::BluetoothNimbleStackInicializator bluetoothStackInicializator;
    // bluetoothStackInicializator.initialize();
}