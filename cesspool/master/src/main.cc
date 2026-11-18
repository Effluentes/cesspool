#include <runtime/composite_root/app/App.hh>
#include <runtime/composite_root/system_orchestrator/SystemOrchestrator.hh>
#include <cstdio>
#include <esp32/configuration/configuration/ConfigurationLoader.hh>

extern "C" void app_main()
{
    printf("Starting Cesspool application...\n");
    static esp32::configuration::ConfigurationLoader esp32ConfigurationLoader;
    static configuration::interface::ConfigurationLoader &configurationLoader = esp32ConfigurationLoader;
    // static hardware_abstraction_layer::interface::PhysicalInterface i2cForDisplay = esp32::hardware_abstraction_layer::I2C(Pin{21}, Pin{22});
    // static human_machine_interface::display::interface::Display display = esp32::human_machine_interface::display::Display(i2cForDisplay);
    static runtime::composite_root::SystemOrchestrator orchestrator(configurationLoader);
    static runtime::composite_root::App app(orchestrator);
    app.run();
}