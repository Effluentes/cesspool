#include "composite_root/app/App.hh"

namespace composite_root
{
    App::App(SystemOrchestrator &orchestrator) : orchestrator_(orchestrator) {}
    void App::run()
    {
        auto result = orchestrator_.orchestrate();
        if (!result)
        {
            // Handle the error case, e.g., log the error or take appropriate action
            // For example, you can log the error status:
            // std::cerr << "Orchestration failed with status: " << static_cast<int>(result.error()) << std::endl;
        }
    }
}