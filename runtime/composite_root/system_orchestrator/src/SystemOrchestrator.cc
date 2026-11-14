#include "runtime/composite_root/system_orchestrator/SystemOrchestrator.hh"

namespace runtime::composite_root
{
    SystemOrchestrator::SystemOrchestrator()
    {
    }

    std::expected<void, SystemOrchestratorErrorStatus> SystemOrchestrator::orchestrate()
    {
        systemAO_.start();

        return std::expected<void, SystemOrchestratorErrorStatus>();
    }
}