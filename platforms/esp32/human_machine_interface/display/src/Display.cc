#include "esp32/human_machine_interface/display/Display.hh"

#include <esp_err.h>
#include <u8g2.h>
#include <esp32_hw_i2c.h>

namespace
{
    void drawCentered(u8g2_t &u8g2, const char *text, uint8_t y)
    {
        const auto width = u8g2_GetStrWidth(&u8g2, text);
        u8g2_DrawStr(&u8g2, (128 - width) / 2, y, text);
    }
}

namespace esp32::human_machine_interface
{
    struct Display::Impl
    {
        u8g2_esp32_i2c_ctx_t i2cContext{};
        u8g2_t u8g2{};

        Impl()
        {
            i2cContext.cfg = U8G2_ESP32_I2C_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(u8g2_esp32_i2c_set_default_context(&i2cContext));

            u8g2_Setup_ssd1306_i2c_128x64_noname_f(
                &u8g2,
                U8G2_R0,
                u8x8_byte_esp32_hw_i2c,
                u8x8_gpio_and_delay_esp32_i2c);
            u8x8_SetI2CAddress(
                &u8g2.u8x8,
                static_cast<uint8_t>(i2cContext.cfg.dev_addr_7bit << 1));
            u8g2_InitDisplay(&u8g2);
            u8g2_SetPowerSave(&u8g2, 0);
        }
    };

    Display::Display()
        : impl_(std::make_unique<Impl>())
    {
    }

    Display::~Display() = default;

    void Display::showScreen(DisplayScreen screen)
    {
        auto &u8g2 = impl_->u8g2;
        u8g2_ClearBuffer(&u8g2);
        u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);

        switch (screen)
        {
        case DisplayScreen::ERROR:
            showErrorScreen();
            break;
        case DisplayScreen::SPLASH:
            showSplashScreen();
            break;
        case DisplayScreen::MAIN_MENU:
            showMainMenuScreen();
            break;
        case DisplayScreen::SETTINGS:
            showSettingsScreen();
            break;
        case DisplayScreen::FIRST_CONFIGURATION:
            showFirstConfigurationScreen();
            break;
        }

        u8g2_SendBuffer(&u8g2);
    }

    void Display::showErrorScreen()
    {
        auto &u8g2 = impl_->u8g2;
        drawCentered(u8g2, "ERROR", 28);
        drawCentered(u8g2, "Check system status", 44);
    }

    void Display::showSplashScreen()
    {
        auto &u8g2 = impl_->u8g2;
        drawCentered(u8g2, "CESSPOOL", 28);
        drawCentered(u8g2, "Starting...", 44);
    }

    void Display::showMainMenuScreen()
    {
        auto &u8g2 = impl_->u8g2;
        u8g2_DrawStr(&u8g2, 0, 12, "MAIN MENU");
        u8g2_DrawHLine(&u8g2, 0, 17, 128);
        u8g2_DrawStr(&u8g2, 0, 32, "System ready");
    }

    void Display::showSettingsScreen()
    {
        auto &u8g2 = impl_->u8g2;
        u8g2_DrawStr(&u8g2, 0, 12, "SETTINGS");
        u8g2_DrawHLine(&u8g2, 0, 17, 128);
    }

    void Display::showFirstConfigurationScreen()
    {
        drawCentered(impl_->u8g2, "CESSPOOL", 36);
    }
}