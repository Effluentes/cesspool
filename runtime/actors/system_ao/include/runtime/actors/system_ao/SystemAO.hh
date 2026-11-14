#pragma once

#include <cstdint>
#include <qpcpp.hpp>
#include <runtime/actors/common/Events.hh>
#include <configuration/configuration_loader/ConfigurationLoader.hh>

namespace runtime::actors
{
    class SystemAO final : public QP::QActive
    {
    public:
        SystemAO(const ConfigurationLoader &config) noexcept;

        SystemAO(SystemAO const &) = delete;
        SystemAO &operator=(SystemAO const &) = delete;

    private:
        Q_STATE_DECL(initial);
        /**
         * @fn boot
         * @brief boot state of the system actor. This state is responsible for initializing the system and transitioning to the operational state.
         */
        Q_STATE_DECL(boot);
        /**
         * @fn operational
         * @brief operational state of the system actor. This state is responsible for managing the system's operation and handling events while the system is running.
         */
        Q_STATE_DECL(operational);

        const ConfigurationLoader &config_;

        QP::QTimeEvt startupTimer_;
    };
    struct LargestEvt : public QP::QEvt
    {
        LargestEvt() : QP::QEvt{static_cast<QP::QSignal>(Events::MAX_SIG), 0, 0} {}
    };

} // namespace runtime::actors
