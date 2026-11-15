#include "esp32/human_machine_interface/display/Display.hh"

#include <cstdio>

namespace esp32::human_machine_interface::display::interface
{
    void Display::showScreen(DisplayScreen screen)
    {
        switch (screen)
        {
        case DisplayScreen::SPLASH:
            std::printf("[Display] Showing SPLASH screen\n");
            break;
        case DisplayScreen::MAIN_MENU:
            std::printf("[Display] Showing MAIN MENU screen\n");
            break;
        case DisplayScreen::SETTINGS:
            std::printf("[Display] Showing SETTINGS screen\n");
            break;
        case DisplayScreen::ERROR:
            std::printf("[Display] Showing ERROR screen\n");
            break;
        default:
            std::printf("[Display] Unknown screen\n");
            break;
        }
    }
}