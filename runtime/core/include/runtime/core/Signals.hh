#pragma once
#include <qpcpp.h>

namespace runtime::core
{
    enum Signals : QP::QSignal
    {
        PLATFORM_READY_SIG = QP::Q_USER_SIG,
        RADIO_READY_SIG,
        SERVER_CONNECTED_SIG,
        SERVER_DISCONNECTED_SIG,
        ALL_SLAVES_AVAILABLE_SIG,
        SLAVE_UNAVAILABLE_SIG
    };
}