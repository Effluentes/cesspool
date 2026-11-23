#include "runtime/actors/provisioning_ao/ProvisioningAO.hh"
#include <cstdio>
#include <runtime/actors/common/Signals.hh>

namespace runtime::actors
{
    ProvisioningAO::ProvisioningAO() noexcept
        : QP::QActive(Q_STATE_CAST(&ProvisioningAO::initial))
    {
    }

    Q_STATE_DEF(ProvisioningAO, initial)
    {
        (void)e;
        std::printf("[ProvisioningAO] Initializing ProvisioningAO\n");
        return tran(&ProvisioningAO::idle);
    }

    Q_STATE_DEF(ProvisioningAO, idle)
    {
        QP::QState status;
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            std::printf("[ProvisioningAO] State: IDLE\n");
            status = Q_HANDLED();
            break;
        }
        case Signals::CONFIGURATION_MISSING_SIG:
        {
            std::printf("[ProvisioningAO] Received CONFIGURATION_MISSING_SIG\n");
            status = tran(&ProvisioningAO::provisioning);
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

    Q_STATE_DEF(ProvisioningAO, provisioning)
    {
        QP::QState status;
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            std::printf("[ProvisioningAO] State: PROVISIONING\n");
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG:
        {
            std::printf("[ProvisioningAO] Exiting PROVISIONING state\n");
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

    Q_STATE_DEF(ProvisioningAO, testProvisioning)
    {
        QP::QState status;
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            std::printf("[ProvisioningAO] State: TEST_PROVISIONING\n");
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG:
        {
            std::printf("[ProvisioningAO] Exiting TEST_PROVISIONING state\n");
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
