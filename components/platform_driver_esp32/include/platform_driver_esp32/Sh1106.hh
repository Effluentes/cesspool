#pragma once
#include <platform_driver_esp32/I2C.hh>
#include <platform/driver/display/interface/Display.hh>

class Sh1106Driver : public platform::driver::interface::Display
{
public:
    Sh1106Driver(Esp32I2CDriver &i2cDriver);
    platform::bsp::InitializeError initialize();

    platform::user_interface::DisplayResult render(const platform::user_interface::ScreenModel  &) override;
    void clear() override;

private:
    Esp32I2CDriver &i2cDriver_;
};