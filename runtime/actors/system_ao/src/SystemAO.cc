
#include "runtime/actors/system_ao/SystemAO.hh"
#include <cstdio>

namespace runtime::actors
{
    SystemAO::SystemAO(DisplayAO &displayAO) noexcept
        : QP::QActive(Q_STATE_CAST(&SystemAO::initial)), displayAO_(displayAO),
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
            std::printf("[SystemAO] Initializing SystemAO\n");
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
            std::printf("[SystemAO] Initializing SystemAO\n");
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
