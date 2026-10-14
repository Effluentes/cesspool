#pragma once
#include <qpcpp.h>

namespace runtime::core
{
    enum Signals : QP::QSignal
    {
        SIGNAL_INIT_OK_SIG = QP::Q_USER_SIG,
        SIGNAL_INIT_FAILED_SIG,
        SIGNAL_FAULT_SIG,
        SIGNAL_RESET_SIG
    };
}