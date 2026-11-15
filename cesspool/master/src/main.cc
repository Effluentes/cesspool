#include <runtime/composite_root/app/App.hh>
#include <runtime/composite_root/system_orchestrator/SystemOrchestrator.hh>
#include <cstdio>

extern "C" void app_main()
{
    printf("Starting Cesspool application...\n");
    static runtime::composite_root::SystemOrchestrator orchestrator;
    static runtime::composite_root::App app(orchestrator);
    app.run();
}