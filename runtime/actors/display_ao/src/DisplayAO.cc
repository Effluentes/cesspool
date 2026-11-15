#include "runtime/actors/display_ao/DisplayAO.hh"

#include <cstdio>
#include <runtime/actors/common/Signals.hh>

namespace runtime::actors
{
    DisplayAO::DisplayAO() noexcept
        : QP::QActive(Q_STATE_CAST(&DisplayAO::initial))
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
            std::printf("[DisplayAO] State: SPLASH\n");
            return Q_HANDLED();

        case DISPLAY_SHOW_SPLASH_SIG:
            std::printf("[DisplayAO] Showing SPLASH screen\n");
            return Q_HANDLED();

        default:
            return super(&QP::QHsm::top);
        }
    }

} // namespace runtime::actors