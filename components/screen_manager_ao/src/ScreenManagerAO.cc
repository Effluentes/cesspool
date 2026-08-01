#include "screen_manager_ao/ScreenManagerAO.hh"

ScreenManagerAO::ScreenManagerAO(Sh1106Driver &oledDriver)
    : QP::QActive(Q_STATE_CAST(&ScreenManagerAO::initial)),
      oledDriver_(oledDriver)
{
}

Q_STATE_DEF(ScreenManagerAO, initial)
{
    (void)e;
    // return tran(&disabled);
    return Q_HANDLED();
}