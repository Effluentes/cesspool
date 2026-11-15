#include "runtime/composite_root/system_orchestrator/SystemOrchestrator.hh"

namespace runtime::composite_root
{
    SystemOrchestrator::SystemOrchestrator()
        : systemAO_(displayAO_.ao)
    {
    }

    std::expected<void, SystemOrchestratorErrorStatus> SystemOrchestrator::orchestrate()
    {
        displayAO_.start();
        systemAO_.start();

        return std::expected<void, SystemOrchestratorErrorStatus>();
    }
}