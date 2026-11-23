#include "runtime/actors/display_ao/DisplayAO.hh"

#include <cstdio>
#include <runtime/actors/common/Signals.hh>

namespace runtime::actors
{
    DisplayAO::DisplayAO(human_machine_interface::interface::Display &display) noexcept
        : QP::QActive(Q_STATE_CAST(&DisplayAO::initial)), display_(display)
    {
    }

    Q_STATE_DEF(DisplayAO, initial)
    {
        return tran(&DisplayAO::splash);
    }

    Q_STATE_DEF(DisplayAO, splash)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            display_.showScreen(human_machine_interface::interface::DisplayScreen::SPLASH);
            return Q_HANDLED();

        case Signals::DISPLAY_SHOW_FIRST_CONFIGURATION_SIG:
            return tran(&DisplayAO::firstConfiguration);

        case Signals::DISPLAY_SHOW_MENU_SIG:
            return tran(&DisplayAO::menu);

        default:
            return super(&QP::QHsm::top);
        }
    }

    Q_STATE_DEF(DisplayAO, firstConfiguration)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            std::printf("[DisplayAO] State: FIRST_CONFIGURATION\n");
            display_.showScreen(human_machine_interface::interface::DisplayScreen::FIRST_CONFIGURATION);
            return Q_HANDLED();

        case Signals::DISPLAY_SHOW_MENU_SIG:
            return tran(&DisplayAO::menu);

        default:
            return super(&QP::QHsm::top);
        }
    }

    Q_STATE_DEF(DisplayAO, menu)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            std::printf("[DisplayAO] State: MENU\n");
            display_.showScreen(human_machine_interface::interface::DisplayScreen::MAIN_MENU);
            return Q_HANDLED();

        case Signals::DISPLAY_SHOW_FIRST_CONFIGURATION_SIG:
            return tran(&DisplayAO::firstConfiguration);

        default:
            return super(&QP::QHsm::top);
        }
    }

} // namespace runtime::actors