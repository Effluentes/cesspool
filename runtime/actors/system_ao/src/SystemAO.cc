
#include "runtime/actors/system_ao/SystemAO.hh"
#include <cstdio>

namespace runtime::actors
{
    SystemAO::SystemAO(
        configuration::interface::ConfigurationLoader &configurationLoader) noexcept
        : QP::QActive(Q_STATE_CAST(&SystemAO::initial)),
          configurationLoader_(configurationLoader),
          startupTimer_(this, STARTUP_TIMEOUT_SIG)
    {
    }

    Q_STATE_DEF(SystemAO, initial)
    {
        return tran(&SystemAO::boot);
    }

    Q_STATE_DEF(SystemAO, boot)
    {
        QP::QState status;
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            std::printf("[SystemAO] State: BOOT\n");
            const auto configuration = configurationLoader_.loadConfiguration();
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG:
        {
            std::printf("[SystemAO] Exiting BOOT state\n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG:
        {
            std::printf("[SystemAO] boot Initializing SystemAO\n");

            status = Q_HANDLED();
            break;
        }
        default:
        {
            status = super(&QP::QHsm::top);
            break;
        }
        }
        return status;
    }
} // namespace runtime::actors
