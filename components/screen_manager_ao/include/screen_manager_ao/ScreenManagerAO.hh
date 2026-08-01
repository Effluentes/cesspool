#pragma once
#include "qpcpp.hpp"
#include <platform_driver_esp32/Sh1106.hh>

class ScreenManagerAO : public QP::QActive
{
public:
    ScreenManagerAO(Sh1106Driver &oledDriver);

protected:
    Q_STATE_DECL(initial);

private:
    Sh1106Driver &oledDriver_;
};