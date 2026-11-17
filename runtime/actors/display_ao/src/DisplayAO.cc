#include "runtime/actors/display_ao/DisplayAO.hh"

#include <runtime/actors/common/Signals.hh>

namespace runtime::actors
{
    DisplayAO::DisplayAO(
        application::Application &application,
        application::DisplayPort display) noexcept
        : QP::QActive(Q_STATE_CAST(&DisplayAO::initial)),
          application_(application),
          display_(display)
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
        case DISPLAY_REFRESH_SIG:
            display_.render(application_.query(application::GetCurrentView{}));
            return Q_HANDLED();
        default:
            return super(&QP::QHsm::top);
        }
    }
}
