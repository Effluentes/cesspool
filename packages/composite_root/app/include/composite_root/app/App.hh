#pragma once
#include <composite_root/system_orchestrator/SystemOrchestrator.hh>

namespace composite_root
{
    class App
    {
    public:
        App(SystemOrchestrator &orchestrator);
        void run();

    private:
        SystemOrchestrator &orchestrator_;
    };
}