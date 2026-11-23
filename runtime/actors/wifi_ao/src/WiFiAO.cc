#include "runtime/actors/wifi_ao/WiFiAO.hh"

#include <cstdio>

namespace runtime::actors
{
    WiFiAO::WiFiAO() noexcept
        : QP::QActive(Q_STATE_CAST(&WiFiAO::initial))
    {
    }

    Q_STATE_DEF(WiFiAO, initial)
    {
        (void)e;
        return tran(&WiFiAO::idle);
    }

    Q_STATE_DEF(WiFiAO, idle)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
            std::printf("[WiFiAO] State: IDLE\n");
            return Q_HANDLED();
        case Signals::
        default:
            return super(&QP::QHsm::top);
        }
    }
}
