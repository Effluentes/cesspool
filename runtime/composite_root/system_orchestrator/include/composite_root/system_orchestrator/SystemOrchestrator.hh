#pragma once
#include <expected>
#include "composite_root/system_orchestrator/SystemOrchestratorErrorStatus.hh"

namespace runtime::composite_root
{
    class SystemOrchestrator
    {
    public:
        SystemOrchestrator() = default;
        std::expected<void, SystemOrchestratorErrorStatus> orchestrate();
    };
}