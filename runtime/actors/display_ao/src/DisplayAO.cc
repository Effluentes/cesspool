#include "runtime/actors/display_ao/DisplayAO.hh"

#include <cstdio>
#include <runtime/actors/common/Signals.hh>

namespace runtime::actors
{
    DisplayAO::DisplayAO(human_machine_interface::display::interface::Display &display) noexcept
        : QP::QActive(Q_STATE_CAST(&DisplayAO::initial)), display_(display)
    {
    }

    Q_STATE_DEF(DisplayAO, initial)
    {
        return tran(&DisplayAO::waiting);
    }

    Q_STATE_DEF(DisplayAO, waiting)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            std::printf("[DisplayAO] Waiting for screen command\n");
            return Q_HANDLED();

        case DISPLAY_SHOW_SPLASH_SIG:
            return tran(&DisplayAO::splash);

        case DISPLAY_SHOW_FIRST_CONFIGURATION_SIG:
            return tran(&DisplayAO::firstConfiguration);

        case DISPLAY_SHOW_MENU_SIG:
            return tran(&DisplayAO::menu);

        case DISPLAY_SHOW_SLAVES_SIG:
            return tran(&DisplayAO::slaves);

        case DISPLAY_SHOW_SETTINGS_SIG:
            return tran(&DisplayAO::settings);

        default:
            return super(&QP::QHsm::top);
        }
    }

    Q_STATE_DEF(DisplayAO, splash)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            std::printf("[DisplayAO] Showing SPLASH screen\n");
            display_.showScreen(human_machine_interface::display::DisplayScreen::SPLASH);
            return Q_HANDLED();

        case DISPLAY_SHOW_FIRST_CONFIGURATION_SIG:
            return tran(&DisplayAO::firstConfiguration);

        case DISPLAY_SHOW_MENU_SIG:
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
            display_.showScreen(human_machine_interface::display::DisplayScreen::FIRST_CONFIGURATION);
            return Q_HANDLED();

        case DISPLAY_SHOW_MENU_SIG:
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
            display_.showMenu(0);
            return Q_HANDLED();

        case DISPLAY_MENU_SELECT_SLAVES_SIG:
            display_.showMenu(0);
            return Q_HANDLED();

        case DISPLAY_MENU_SELECT_SETTINGS_SIG:
            display_.showMenu(1);
            return Q_HANDLED();

        case DISPLAY_SHOW_SLAVES_SIG:
            return tran(&DisplayAO::slaves);

        case DISPLAY_SHOW_FIRST_CONFIGURATION_SIG:
            return tran(&DisplayAO::firstConfiguration);

        case DISPLAY_SHOW_SETTINGS_SIG:
            return tran(&DisplayAO::settings);

        default:
            return super(&QP::QHsm::top);
        }
    }

    Q_STATE_DEF(DisplayAO, slaves)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            display_.showSlaves();
            return Q_HANDLED();

        case DISPLAY_SHOW_MENU_SIG:
            return tran(&DisplayAO::menu);

        default:
            return super(&QP::QHsm::top);
        }
    }

    Q_STATE_DEF(DisplayAO, settings)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            display_.showSettingsMenu(selectedSetting_);
            return Q_HANDLED();

        case DISPLAY_SETTINGS_SELECT_NETWORK_SIG:
            selectedSetting_ = 0;
            display_.showSettingsMenu(selectedSetting_);
            return Q_HANDLED();

        case DISPLAY_SETTINGS_SELECT_DISPLAY_SIG:
            selectedSetting_ = 1;
            display_.showSettingsMenu(selectedSetting_);
            return Q_HANDLED();

        case DISPLAY_SETTINGS_SELECT_ABOUT_SIG:
            selectedSetting_ = 2;
            display_.showSettingsMenu(selectedSetting_);
            return Q_HANDLED();

        case DISPLAY_SHOW_MENU_SIG:
            return tran(&DisplayAO::menu);

        case DISPLAY_SHOW_SETTING_NETWORK_SIG:
            selectedSetting_ = 0;
            return tran(&DisplayAO::settingDetail);

        case DISPLAY_SHOW_SETTING_DISPLAY_SIG:
            selectedSetting_ = 1;
            return tran(&DisplayAO::settingDetail);

        case DISPLAY_SHOW_SETTING_ABOUT_SIG:
            selectedSetting_ = 2;
            return tran(&DisplayAO::settingDetail);

        default:
            return super(&QP::QHsm::top);
        }
    }

    Q_STATE_DEF(DisplayAO, settingDetail)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            display_.showSetting(selectedSetting_);
            return Q_HANDLED();

        case DISPLAY_SHOW_SETTINGS_SIG:
            return tran(&DisplayAO::settings);

        default:
            return super(&QP::QHsm::top);
        }
    }

} // namespace runtime::actors