#include <runtime/composite_root/app/App.hh>
#include <runtime/composite_root/system_orchestrator/SystemOrchestrator.hh>
#include <cstdio>
#include <esp32/configuration/configuration/ConfigurationLoader.hh>
#include <esp32/human_machine_interface/display/Display.hh>

extern "C" void app_main()
{
    printf("Starting Cesspool application...\n");
    static esp32::configuration::ConfigurationLoader esp32ConfigurationLoader;
    static configuration::interface::ConfigurationLoader &configurationLoader = esp32ConfigurationLoader;
    static esp32::human_machine_interface::Display esp32Display;
    static human_machine_interface::interface::Display &display = esp32Display;
    static runtime::composite_root::SystemOrchestrator orchestrator(configurationLoader, display);
    static runtime::composite_root::App app(orchestrator);
    app.run();
}