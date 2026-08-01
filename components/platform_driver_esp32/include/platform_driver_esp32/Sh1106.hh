#pragma once
#include <platform_driver_esp32/I2C.hh>

class Sh1106Driver
{
public:
    Sh1106Driver(Esp32I2CDriver &i2cDriver);
    platform::bsp::InitializeError initialize();

private:
    Esp32I2CDriver &i2cDriver_;
};