#pragma once
#include <platform/driver/i2c/interface/I2C.hh>

class Esp32I2CDriver : public platform::driver::interface::I2C
{
    Esp32I2CDriver();

    platform::bsp::InitializeError initialize() override;
};