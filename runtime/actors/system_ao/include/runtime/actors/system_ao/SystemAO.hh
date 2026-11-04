#pragma once

#include <cstdint>
#include "qpcpp.hpp"

// #include "Signals.hh"

namespace runtime::actors
{
    enum Signals : QP::QSignal
    {

        PLATFORM_READY_SIG = QP::Q_USER_SIG,
        RADIO_READY_SIG,

        SERVER_CONNECTED_SIG,
        SERVER_DISCONNECTED_SIG,

        ALL_SLAVES_AVAILABLE_SIG,
        SLAVE_UNAVAILABLE_SIG,

        STARTUP_TIMEOUT_SIG,
        SYSTEM_RESTART_REQUEST_SIG,

        // ALWAYS LAST
        MAX_SIG
    };
    class SystemAO final : public QP::QActive
    {
    public:
        SystemAO() noexcept;

        SystemAO(SystemAO const &) = delete;
        SystemAO &operator=(SystemAO const &) = delete;

    private:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(starting);
        Q_STATE_DECL(operational);
        Q_STATE_DECL(fully_operational);
        Q_STATE_DECL(degraded);
        Q_STATE_DECL(fault);

        bool platformReady_{false};
        bool radioReady_{false};
        bool serverConnected_{false};
        bool allSlavesAvailable_{false};

        QP::QTimeEvt startupTimer_;
    };
    struct LargestEvt : public QP::QEvt
    {
        LargestEvt() : QP::QEvt{static_cast<QP::QSignal>(Signals::MAX_SIG), 0, 0} {}
    };

} // namespace runtime::actors
