#pragma once
#include <platform/user_interface/screen_model/ScreenModel.hh>

namespace platform::driver::interface
{
    struct Display
    {
        virtual ~Display() = default;

        virtual platform::user_interface::DisplayResult render(const platform::user_interface::ScreenModel  &) = 0;
        virtual void clear() = 0;
    };
}