#pragma once

#include <qpcpp.hpp>

namespace runtime::actors
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
    class SystemAO final : public QP::QActive
    {
    public:
        SystemAO() noexcept;

    protected:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(starting);

        Q_STATE_DECL(operational);
        Q_STATE_DECL(fully_operational);
        Q_STATE_DECL(degraded);

    private:
        bool platformReady_{false};
        bool radioReady_{false};
        bool serverConnected_{false};
        bool allSlavesAvailable_{false};
    };
}