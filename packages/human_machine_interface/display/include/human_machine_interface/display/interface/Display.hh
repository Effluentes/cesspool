#pragma once

namespace human_machine_interface::interface
{
    enum class DisplayScreen
    {
        ERROR,
        SPLASH,
        MAIN_MENU,
        SETTINGS,
        FIRST_CONFIGURATION
    };

    class Display
    {
    public:
        virtual ~Display() = default;

        virtual void showScreen(DisplayScreen) = 0;
    };
}
