#include "runtime/composite_root/system_orchestrator/SystemOrchestrator.hh"

#include <esp_log.h>

namespace runtime::composite_root
{
    namespace
    {
        constexpr char logTag[] = "SystemOrchestrator";
    }

    SystemOrchestrator::SystemOrchestrator(
        configuration::interface::ConfigurationLoader &configurationLoader,
        human_machine_interface::display::interface::Display &display)
        : displayAO_(display), systemAO_(displayAO_.ao, configurationLoader)
    {
    }

    std::expected<void, SystemOrchestratorErrorStatus> SystemOrchestrator::orchestrate()
    {
        displayAO_.start();
        systemAO_.start();

        const auto controlsResult = controls_.start(systemAO_.ao);
        if (controlsResult != ESP_OK)
        {
            ESP_LOGE(logTag, "Failed to start controls: %s", esp_err_to_name(controlsResult));
            return std::unexpected(SystemOrchestratorErrorStatus::unknown_failure);
        }

        return std::expected<void, SystemOrchestratorErrorStatus>();
    }
}