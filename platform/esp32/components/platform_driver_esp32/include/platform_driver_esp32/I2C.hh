#pragma once
#include <driver/gpio.h>
#include <platform/driver/i2c/interface/I2C.hh>
#include <platform_driver_esp32/GPIO.hh>

struct Esp32I2CWires
{
    Esp32GPIODriver sda;
    Esp32GPIODriver scl;
};

class Esp32I2CDriver : public platform::driver::interface::I2C<Esp32I2CDriver>
{
    friend class platform::driver::interface::I2C<Esp32I2CDriver>;

public:
    Esp32I2CDriver(Esp32I2CWires);

private:
    platform::bsp::InitializeError initializeImpl();
    void writeImpl(const platform::driver::Buffer &data);
    platform::driver::Buffer readImpl();
    void writeRegisterImpl(const platform::driver::RegisterAddress registerAddress, const platform::driver::Buffer &data);
    platform::driver::Buffer readRegisterImpl(const platform::driver::RegisterAddress registerAddress);
    bool isDeviceConnectedImpl();

    Esp32I2CWires i2cWires_;
};
