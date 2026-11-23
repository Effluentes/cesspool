#pragma once

#include <qpcpp.hpp>

namespace runtime::actors
{
    enum Signals : QP::QSignal
    {
        DISPLAY_SHOW_FIRST_CONFIGURATION_SIG = QP::Q_USER_SIG,
        DISPLAY_SHOW_MENU_SIG,
        CONFIGURATION_MISSING_SIG,
        MAX_SIG
    };
}
