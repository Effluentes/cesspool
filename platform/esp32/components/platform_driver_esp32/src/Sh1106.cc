#include "platform_driver_esp32/Sh1106.hh"

Sh1106Driver::Sh1106Driver(Esp32I2CDriver &i2cDriver)
    : i2cDriver_(i2cDriver)
{
}

platform::bsp::InitializeError Sh1106Driver::initialize()
{
    return i2cDriver_.initialize();
}

platform::user_interface::DisplayResult Sh1106Driver::render(platform::user_interface::ScreenModel const &model)
{
    // Implementation for rendering the screen model on the SH1106 display
    return platform::user_interface::DisplayResult{std::in_place};
}

void Sh1106Driver::clear()
{
    // Implementation for clearing the SH1106 display
}