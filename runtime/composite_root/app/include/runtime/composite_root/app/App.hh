#pragma once
#include <expected>
#include <runtime/composite_root/system_orchestrator/SystemOrchestrator.hh>
#include <qpcpp.hpp>

namespace runtime::composite_root
{
    class App
    {
    public:
        App(SystemOrchestrator &orchestrator);
        std::expected<void, SystemOrchestratorErrorStatus> run();

    private:
        SystemOrchestrator &orchestrator_;
    };
}
