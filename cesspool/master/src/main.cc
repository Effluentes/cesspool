#include <runtime/composite_root/app/App.hh>
#include <runtime/composite_root/system_orchestrator/SystemOrchestrator.hh>
#include "esp32/human_machine_interface/display/Display.hh"
#include <cstdio>

namespace
{
    class DummyConfigurationLoader final : public configuration::interface::ConfigurationLoader
    {
    public:
        std::expected<configuration::Configuration, configuration::interface::ConfigurationLoaderError>
        loadConfiguration() override
        {
            return std::unexpected(
                configuration::interface::ConfigurationLoaderError::CONFIGURATION_NOT_FOUND);
        }
    };
}

extern "C" void app_main()
{
    printf("Starting Cesspool application...\n");
    static DummyConfigurationLoader configurationLoader;
    static esp32::human_machine_interface::display::interface::Display display;
    static runtime::composite_root::SystemOrchestrator orchestrator(configurationLoader, display);
    static runtime::composite_root::App app(orchestrator);
    app.run();
}