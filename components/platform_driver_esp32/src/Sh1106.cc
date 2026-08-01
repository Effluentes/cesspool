#include "platform_driver_esp32/Sh1106.hh"

Sh1106Driver::Sh1106Driver(Esp32I2CDriver &i2cDriver)
    : i2cDriver_(i2cDriver)
{
}

platform::bsp::InitializeError Sh1106Driver::initialize()
{
    return i2cDriver_.initialize();
}