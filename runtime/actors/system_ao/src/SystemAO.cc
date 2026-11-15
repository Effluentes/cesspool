
#include "runtime/actors/system_ao/SystemAO.hh"
#include <cstdio>

namespace runtime::actors
{
    SystemAO::SystemAO(
        DisplayAO &displayAO,
        configuration::interface::ConfigurationLoader &configurationLoader) noexcept
        : QP::QActive(Q_STATE_CAST(&SystemAO::initial)), displayAO_(displayAO),
          configurationLoader_(configurationLoader),
          startupTimer_(this, STARTUP_TIMEOUT_SIG)
    {
    }

    Q_STATE_DEF(SystemAO, initial)
    {
        return tran(&SystemAO::boot);
    }

    Q_STATE_DEF(SystemAO, boot)
    {
        QP::QState status;
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            std::printf("[SystemAO] State: BOOT\n");
            static QP::QEvt const showSplashEvent{DISPLAY_SHOW_SPLASH_SIG, 0, 0};
            displayAO_.POST(&showSplashEvent, this);
            const auto configuration = configurationLoader_.loadConfiguration();
            if (configuration)
            {
                configurationLoaded_ = true;
            }
            else
            {
                if (configuration.error() ==
                    configuration::interface::ConfigurationLoaderError::CONFIGURATION_NOT_FOUND)
                {
                    configurationMissing_ = true;
                }
                else
                {
                    std::printf(
                        "[SystemAO] Configuration load failed with error %u\n",
                        static_cast<unsigned>(configuration.error()));
                }
            }
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG:
        {
            std::printf("[SystemAO] Exiting BOOT state\n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG:
        {
            std::printf("[SystemAO] boot Initializing SystemAO\n");
            if (configurationMissing_)
            {
                return tran(&SystemAO::firstConfiguration);
            }
            if (configurationLoaded_)
            {
                return tran(&SystemAO::operational);
            }
            status = Q_HANDLED();
            break;
        }
        default:
        {
            status = super(&QP::QHsm::top);
            break;
        }
        }
        return status;
    }

    Q_STATE_DEF(SystemAO, firstConfiguration)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            static QP::QEvt const showEvent{DISPLAY_SHOW_FIRST_CONFIGURATION_SIG, 0, 0};
            displayAO_.POST(&showEvent, this);
            return Q_HANDLED();
        }
        case CONTROL_SELECT_SIG:
        case CONTROL_CONFIRM_SIG:
            return tran(&SystemAO::menu);
        case CONTROL_BACK_SIG:
            return Q_HANDLED();
        default:
            return super(&SystemAO::boot);
        }
    }

    Q_STATE_DEF(SystemAO, menu)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            selectedMenuItem_ = 0;
            static QP::QEvt const showEvent{DISPLAY_SHOW_MENU_SIG, 0, 0};
            displayAO_.POST(&showEvent, this);
            return Q_HANDLED();
        }
        case CONTROL_ROTATE_RIGHT_SIG:
        {
            selectedMenuItem_ = static_cast<std::uint8_t>((selectedMenuItem_ + 1U) % 2U);
            static QP::QEvt const slavesSelectedEvent{DISPLAY_MENU_SELECT_SLAVES_SIG, 0, 0};
            static QP::QEvt const settingsSelectedEvent{DISPLAY_MENU_SELECT_SETTINGS_SIG, 0, 0};
            displayAO_.POST(
                selectedMenuItem_ == 0 ? &slavesSelectedEvent : &settingsSelectedEvent,
                this);
            return Q_HANDLED();
        }
        case CONTROL_ROTATE_LEFT_SIG:
        {
            selectedMenuItem_ = static_cast<std::uint8_t>((selectedMenuItem_ + 1U) % 2U);
            static QP::QEvt const slavesSelectedEvent{DISPLAY_MENU_SELECT_SLAVES_SIG, 0, 0};
            static QP::QEvt const settingsSelectedEvent{DISPLAY_MENU_SELECT_SETTINGS_SIG, 0, 0};
            displayAO_.POST(
                selectedMenuItem_ == 0 ? &slavesSelectedEvent : &settingsSelectedEvent,
                this);
            return Q_HANDLED();
        }
        case CONTROL_SELECT_SIG:
        case CONTROL_CONFIRM_SIG:
            if (selectedMenuItem_ == 0)
            {
                return tran(&SystemAO::slaves);
            }
            selectedSetting_ = 0;
            return tran(&SystemAO::settings);
        case CONTROL_BACK_SIG:
            return tran(&SystemAO::firstConfiguration);
        default:
            return super(&SystemAO::boot);
        }
    }

    Q_STATE_DEF(SystemAO, slaves)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            static QP::QEvt const showEvent{DISPLAY_SHOW_SLAVES_SIG, 0, 0};
            displayAO_.POST(&showEvent, this);
            return Q_HANDLED();
        }
        case CONTROL_BACK_SIG:
            return tran(&SystemAO::menu);
        default:
            return super(&SystemAO::boot);
        }
    }

    Q_STATE_DEF(SystemAO, settings)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            static QP::QEvt const showEvent{DISPLAY_SHOW_SETTINGS_SIG, 0, 0};
            displayAO_.POST(&showEvent, this);
            static QP::QEvt const networkSelectedEvent{DISPLAY_SETTINGS_SELECT_NETWORK_SIG, 0, 0};
            static QP::QEvt const displaySelectedEvent{DISPLAY_SETTINGS_SELECT_DISPLAY_SIG, 0, 0};
            static QP::QEvt const aboutSelectedEvent{DISPLAY_SETTINGS_SELECT_ABOUT_SIG, 0, 0};
            displayAO_.POST(
                selectedSetting_ == 0 ? &networkSelectedEvent
                                      : selectedSetting_ == 1 ? &displaySelectedEvent
                                                             : &aboutSelectedEvent,
                this);
            return Q_HANDLED();
        }
        case CONTROL_ROTATE_RIGHT_SIG:
            selectedSetting_ = static_cast<std::uint8_t>((selectedSetting_ + 1U) % 3U);
            break;
        case CONTROL_ROTATE_LEFT_SIG:
            selectedSetting_ = static_cast<std::uint8_t>((selectedSetting_ + 2U) % 3U);
            break;
        case CONTROL_SELECT_SIG:
        case CONTROL_CONFIRM_SIG:
        {
            static QP::QEvt const networkSelectedEvent{DISPLAY_SHOW_SETTING_NETWORK_SIG, 0, 0};
            static QP::QEvt const displaySelectedEvent{DISPLAY_SHOW_SETTING_DISPLAY_SIG, 0, 0};
            static QP::QEvt const aboutSelectedEvent{DISPLAY_SHOW_SETTING_ABOUT_SIG, 0, 0};
            displayAO_.POST(
                selectedSetting_ == 0 ? &networkSelectedEvent
                                      : selectedSetting_ == 1 ? &displaySelectedEvent
                                                             : &aboutSelectedEvent,
                this);
            return tran(&SystemAO::settingDetail);
        }
        case CONTROL_BACK_SIG:
            return tran(&SystemAO::menu);
        default:
            return super(&SystemAO::boot);
        }

        static QP::QEvt const networkSelectedEvent{DISPLAY_SETTINGS_SELECT_NETWORK_SIG, 0, 0};
        static QP::QEvt const displaySelectedEvent{DISPLAY_SETTINGS_SELECT_DISPLAY_SIG, 0, 0};
        static QP::QEvt const aboutSelectedEvent{DISPLAY_SETTINGS_SELECT_ABOUT_SIG, 0, 0};
        displayAO_.POST(
            selectedSetting_ == 0 ? &networkSelectedEvent
                                  : selectedSetting_ == 1 ? &displaySelectedEvent
                                                         : &aboutSelectedEvent,
            this);
        return Q_HANDLED();
    }

    Q_STATE_DEF(SystemAO, settingDetail)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            return Q_HANDLED();
        case CONTROL_BACK_SIG:
            return tran(&SystemAO::settings);
        default:
            return super(&SystemAO::boot);
        }
    }

    Q_STATE_DEF(SystemAO, operational)
    {
        QP::QState status;
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            std::printf("[SystemAO] State: OPERATIONAL\n");
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG:
        {
            std::printf("[SystemAO] Exiting OPERATIONAL state\n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG:
        {
            std::printf("[SystemAO] operational Initializing SystemAO\n");
            status = Q_HANDLED();
            break;
        }
        default:
        {
            status = super(&SystemAO::boot);
            break;
        }
        }
        return status;
    }

    // // =========================================================
    // // STARTING
    // // =========================================================

    // Q_STATE_DEF(SystemAO, starting)
    // {
    //     switch (e->sig)
    //     {

    //     case Q_ENTRY_SIG:
    //         std::printf("[SystemAO] State: STARTING\n");
    //         platformReady_ = false;
    //         radioReady_ = false;
    //         serverConnected_ = false;
    //         allSlavesAvailable_ = false;

    //         startupTimer_.armX(kStartupTimeoutTicks);
    //         return Q_HANDLED();

    //     case Q_EXIT_SIG:
    //         startupTimer_.disarm();
    //         return Q_HANDLED();

    //     case Events::PLATFORM_READY_SIG:
    //         if (platformReady_)
    //         {
    //             return Q_HANDLED();
    //         }

    //         std::printf("[SystemAO] Platform ready\n");
    //         platformReady_ = true;
    //         break;

    //     case Events::RADIO_READY_SIG:
    //         if (radioReady_)
    //         {
    //             return Q_HANDLED();
    //         }

    //         std::printf("[SystemAO] Radio ready\n");
    //         radioReady_ = true;
    //         break;

    //     case Events::SERVER_CONNECTED_SIG:
    //         serverConnected_ = true;
    //         return Q_HANDLED();

    //     case Events::SERVER_DISCONNECTED_SIG:
    //         serverConnected_ = false;
    //         return Q_HANDLED();

    //     case Events::ALL_SLAVES_AVAILABLE_SIG:
    //         allSlavesAvailable_ = true;
    //         return Q_HANDLED();

    //     case Events::SLAVE_UNAVAILABLE_SIG:
    //         allSlavesAvailable_ = false;
    //         return Q_HANDLED();

    //     case Events::STARTUP_TIMEOUT_SIG:
    //         return tran(&SystemAO::fault);

    //     default:
    //         return super(&QP::QHsm::top);
    //     }

    //     if (platformReady_ && radioReady_)
    //     {
    //         return tran(&SystemAO::operational);
    //     }

    //     return Q_HANDLED();
    // }

} // namespace runtime::actors
