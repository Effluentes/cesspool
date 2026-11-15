#pragma once

namespace human_machine_interface::display::interface
{
    enum class DisplayScreen
    {
        SPLASH,
        MAIN_MENU,
        SETTINGS,
        ERROR
    };

    class Display
    {
    public:
        virtual ~Display() = default;

        virtual void showScreen(DisplayScreen screen) = 0;
    };
}