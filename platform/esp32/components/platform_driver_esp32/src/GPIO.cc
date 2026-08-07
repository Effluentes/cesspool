#include "platform_driver_esp32/GPIO.hh"

namespace
{
    const int high = 1;
    const int low = 0;
}

Esp32GPIODriver::Esp32GPIODriver(gpio_num_t gpio_num) : pin_(gpio_num) {
    gpio_reset_pin(pin_);
    gpio_set_direction(pin_, GPIO_MODE_OUTPUT);
}

void Esp32GPIODriver::setPinHigh() {
    setPinHighImpl();
}

void Esp32GPIODriver::setPinLow() {
    setPinLowImpl();
}

bool Esp32GPIODriver::readPin()const {
    return readPinImpl();
}

void Esp32GPIODriver::setPinHighImpl() {
    gpio_set_level(pin_, high);
}

void Esp32GPIODriver::setPinLowImpl() {
    gpio_set_level(pin_, low);
}

bool Esp32GPIODriver::readPinImpl() const {
    return gpio_get_level(pin_) != low;
}