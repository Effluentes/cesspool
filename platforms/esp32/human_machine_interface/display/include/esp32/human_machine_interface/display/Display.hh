#pragma once

#include <runtime/application/DisplayPort.hh>

#include <esp32_hw_i2c.h>
#include <u8g2.h>

namespace esp32::human_machine_interface::display::interface
{
    class Display final
    {
    public:
        Display();

        void render(runtime::application::View const &view);

    private:
        u8g2_esp32_i2c_ctx_t i2cContext_{};
        u8g2_t u8g2_{};
    };
}
