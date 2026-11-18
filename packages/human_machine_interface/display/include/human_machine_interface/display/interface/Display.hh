#pragma once

#include <cstdint>

namespace human_machine_interface::display::interface
{
    enum class DisplayScreen
    {
        ERROR,
        SPLASH,
        MAIN_MENU,
        SETTINGS,
        FIRST_CONFIGURATION
    };
    namespace interface
    {
        class Display
        {
        public:
            virtual ~Display() = default;

            virtual void showScreen(DisplayScreen screen) = 0;
            virtual void showMenu(std::uint8_t selectedItem) = 0;
            virtual void showSlaves() = 0;
            virtual void showSettingsMenu(std::uint8_t selectedItem) = 0;
            virtual void showSetting(std::uint8_t setting) = 0;
        };
    }
}
