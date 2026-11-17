#pragma once

#include <qpcpp.hpp>

namespace runtime::actors
{
    enum Signals : QP::QSignal
    {
        DISPLAY_REFRESH_SIG = QP::Q_USER_SIG,
        CONTROL_ROTATE_LEFT_SIG,
        CONTROL_ROTATE_RIGHT_SIG,
        CONTROL_SELECT_SIG,
        CONTROL_CONFIRM_SIG,
        CONTROL_BACK_SIG,
        MAX_SIG
    };
}
