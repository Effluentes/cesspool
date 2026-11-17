#include <runtime/composite_root/app/App.hh>
#include <runtime/composite_root/system_orchestrator/SystemOrchestrator.hh>

#include "esp32/configuration/configuration/ConfigurationLoader.hh"
#include "esp32/human_machine_interface/controls/Controls.hh"
#include "esp32/human_machine_interface/display/Display.hh"
#include "observability/Esp32Logger.hh"

extern "C" void app_main()
{
    static Esp32Logger logger;
    static esp32::configuration::ConfigurationLoader configurationLoader;
    static esp32::human_machine_interface::controls::Controls controls;
    static esp32::human_machine_interface::display::interface::Display display;
    static runtime::composite_root::SystemOrchestrator orchestrator(
        configuration::interface::ConfigurationLoader::bind(configurationLoader),
        runtime::application::DisplayPort::bind(display),
        runtime::application::InputPort::bind(controls),
        runtime::application::LogPort::bind(logger));
    static runtime::composite_root::App app(orchestrator);
    if (!app.run())
    {
        logger.write(runtime::application::LogLevel::error, "Application startup failed");
    }
}
