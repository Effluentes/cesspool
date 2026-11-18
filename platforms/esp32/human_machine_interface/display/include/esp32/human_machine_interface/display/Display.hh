#pragma once

#include <human_machine_interface/display/interface/Display.hh>
#include <memory>

namespace esp32::human_machine_interface::display::interface
{
    using ::human_machine_interface::display::DisplayScreen;

    class Display : public ::human_machine_interface::display::interface::Display
    {
    public:
        Display();
        ~Display() override;

        virtual void showScreen(DisplayScreen screen) override;
        void showMenu(std::uint8_t selectedItem) override;
        void showSlaves() override;
        void showSettingsMenu(std::uint8_t selectedItem) override;
        void showSetting(std::uint8_t setting) override;
    };
}