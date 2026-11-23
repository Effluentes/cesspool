#pragma once

#include <human_machine_interface/display/interface/Display.hh>
#include <memory>

namespace esp32::human_machine_interface
{
    using ::human_machine_interface::interface::DisplayScreen;

    class Display : public ::human_machine_interface::interface::Display
    {
    public:
        Display();
        ~Display() override;

        void showScreen(DisplayScreen screen) override;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;

        void showErrorScreen();
        void showSplashScreen();
        void showMainMenuScreen();
        void showSettingsScreen();
        void showFirstConfigurationScreen();
    };
}