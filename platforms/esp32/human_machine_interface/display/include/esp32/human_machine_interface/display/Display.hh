#pragma once
#include <human_machine_interface/display/interface/Display.hh>

namespace esp32::human_machine_interface::display::interface
{
    using ::human_machine_interface::display::interface::DisplayScreen;

    class Display : public ::human_machine_interface::display::interface::Display
    {
    public:
        virtual ~Display() = default;

        virtual void showScreen(DisplayScreen screen) override;
    };
}