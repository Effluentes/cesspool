#include "runtime/actors/system_ao/SystemAO.hh"

namespace runtime::actors
{
    SystemAO::SystemAO() noexcept
        : QP::QActive(Q_STATE_CAST(&SystemAO::initial))
    {
    }

    // ---------------------------------------------------------
    // initial
    // ---------------------------------------------------------

    Q_STATE_DEF(SystemAO, initial)
    {
        (void)e;
        return tran(&SystemAO::starting);
    }

    // ---------------------------------------------------------
    // starting
    // ---------------------------------------------------------

    Q_STATE_DEF(SystemAO, starting)
    {
        switch (e->sig)
        {
        case PLATFORM_READY_SIG:
            platformReady_ = true;
            break;

        case RADIO_READY_SIG:
            radioReady_ = true;
            break;

        default:
            return super(&QP::QHsm::top);
        }

        if (platformReady_ && radioReady_)
        {
            return tran(&SystemAO::operational);
        }

        return Q_HANDLED();
    }

    // ---------------------------------------------------------
    // operational (superstate)
    // ---------------------------------------------------------

    Q_STATE_DEF(SystemAO, operational)
    {
        switch (e->sig)
        {
        case Q_INIT_SIG:
            if (serverConnected_ && allSlavesAvailable_)
            {
                return tran(&SystemAO::fully_operational);
            }

            return tran(&SystemAO::degraded);

        case SERVER_CONNECTED_SIG:
            serverConnected_ = true;

            if (allSlavesAvailable_)
            {
                return tran(&SystemAO::fully_operational);
            }

            return Q_HANDLED();

        case SERVER_DISCONNECTED_SIG:
            serverConnected_ = false;
            return tran(&SystemAO::degraded);

        case ALL_SLAVES_AVAILABLE_SIG:
            allSlavesAvailable_ = true;

            if (serverConnected_)
            {
                return tran(&SystemAO::fully_operational);
            }

            return Q_HANDLED();

        case SLAVE_UNAVAILABLE_SIG:
            allSlavesAvailable_ = false;
            return tran(&SystemAO::degraded);

        default:
            return super(&QP::QHsm::top);
        }
    }

    // ---------------------------------------------------------
    // fully_operational (substate of operational)
    // ---------------------------------------------------------

    Q_STATE_DEF(SystemAO, fully_operational)
    {
        switch (e->sig)
        {
        default:
            return super(&SystemAO::operational);
        }
    }

    // ---------------------------------------------------------
    // degraded (substate of operational)
    // ---------------------------------------------------------

    Q_STATE_DEF(SystemAO, degraded)
    {
        switch (e->sig)
        {
        default:
            return super(&SystemAO::operational);
        }
    }
}