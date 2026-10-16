#include <composite_root/app/App.hh>
#include <composite_root/system_orchestrator/SystemOrchestrator.hh>

extern "C" void app_main()
{
    composite_root::SystemOrchestrator orchestrator;
    composite_root::App app(orchestrator);
    app.run();
}