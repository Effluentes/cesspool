#include "esp32/human_machine_interface/display/Display.hh"

#include <esp_err.h>

namespace
{
    using runtime::application::Page;
    using runtime::application::View;

    void drawCentered(u8g2_t &u8g2, const char *text, std::uint8_t y)
    {
        const auto width = u8g2_GetStrWidth(&u8g2, text);
        u8g2_DrawStr(&u8g2, (128 - width) / 2, y, text);
    }

    void drawMenu(u8g2_t &u8g2, char const *title, char const *const *items,
                  std::uint8_t count, std::uint8_t selected)
    {
        u8g2_DrawStr(&u8g2, 0, 10, title);
        u8g2_DrawHLine(&u8g2, 0, 14, 128);
        for (std::uint8_t index = 0; index < count; ++index)
        {
            const auto y = static_cast<std::uint8_t>(28 + index * 12);
            u8g2_DrawStr(&u8g2, 2, y, index == selected ? ">" : " ");
            u8g2_DrawStr(&u8g2, 12, y, items[index]);
        }
    }
}

namespace esp32::human_machine_interface::display::interface
{
    Display::Display()
    {
        i2cContext_.cfg = U8G2_ESP32_I2C_CONFIG_DEFAULT();
        i2cContext_.cfg.sda_pin = 4;
        i2cContext_.cfg.scl_pin = 5;
        ESP_ERROR_CHECK(u8g2_esp32_i2c_set_default_context(&i2cContext_));

        u8g2_Setup_ssd1306_i2c_128x64_noname_f(
            &u8g2_,
            U8G2_R0,
            u8x8_byte_esp32_hw_i2c,
            u8x8_gpio_and_delay_esp32_i2c);
        u8x8_SetI2CAddress(
            &u8g2_.u8x8,
            static_cast<std::uint8_t>(i2cContext_.cfg.dev_addr_7bit << 1));
        u8g2_InitDisplay(&u8g2_);
        u8g2_SetPowerSave(&u8g2_, 0);
    }

    void Display::render(runtime::application::View const &view)
    {
        u8g2_ClearBuffer(&u8g2_);
        u8g2_SetFont(&u8g2_, u8g2_font_6x10_tf);

        switch (view.page)
        {
        case Page::splash:
            drawCentered(u8g2_, "CESSPOOL", 28);
            drawCentered(u8g2_, "Starting...", 44);
            break;
        case Page::firstConfiguration:
            drawCentered(u8g2_, "CESSPOOL", 28);
            drawCentered(u8g2_, "Press to configure", 46);
            break;
        case Page::mainMenu:
        {
            constexpr char const *items[]{"Slaves", "Settings"};
            drawMenu(u8g2_, "MAIN MENU", items, 2, view.selectedIndex);
            break;
        }
        case Page::slaves:
            u8g2_DrawStr(&u8g2_, 0, 10, "SLAVES");
            u8g2_DrawHLine(&u8g2_, 0, 14, 128);
            u8g2_DrawStr(&u8g2_, 0, 32, "No devices found");
            u8g2_DrawStr(&u8g2_, 0, 60, "Blue: back");
            break;
        case Page::settingsMenu:
        {
            constexpr char const *items[]{"Network", "Display", "About"};
            drawMenu(u8g2_, "SETTINGS", items, 3, view.selectedIndex);
            break;
        }
        case Page::settingDetails:
            u8g2_DrawStr(&u8g2_, 0, 10, "SETTING");
            u8g2_DrawHLine(&u8g2_, 0, 14, 128);
            switch (view.selectedIndex)
            {
            case 0:
                u8g2_DrawStr(&u8g2_, 0, 32, "Network settings");
                break;
            case 1:
                u8g2_DrawStr(&u8g2_, 0, 32, "Display settings");
                break;
            default:
                u8g2_DrawStr(&u8g2_, 0, 32, "About Cesspool");
                break;
            }
            u8g2_DrawStr(&u8g2_, 0, 60, "Blue: back");
            break;
        case Page::operational:
            drawCentered(u8g2_, "CESSPOOL", 28);
            drawCentered(u8g2_, "System ready", 44);
            break;
        case Page::error:
            drawCentered(u8g2_, "ERROR", 28);
            drawCentered(u8g2_, "Configuration failed", 44);
            break;
        }

        u8g2_SendBuffer(&u8g2_);
    }
}
