#pragma once

#include <qpcpp.hpp>

namespace runtime::actors
{
    enum Events : QP::QSignal
    {
        PLATFORM_READY_SIG = QP::Q_USER_SIG,
        RADIO_READY_SIG,

        SERVER_CONNECTED_SIG,
        SERVER_DISCONNECTED_SIG,

        ALL_SLAVES_AVAILABLE_SIG,
        SLAVE_UNAVAILABLE_SIG,

        NETWORK_CONNECT_REQUEST_SIG,
        NETWORK_DISCONNECT_REQUEST_SIG,

        STARTUP_TIMEOUT_SIG,
        SYSTEM_RESTART_REQUEST_SIG,

        MAX_SIG
    };
}
