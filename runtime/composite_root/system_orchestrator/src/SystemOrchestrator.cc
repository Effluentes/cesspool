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
        human_machine_interface::interface::Display &display)
        : provisioningAO_(),
          systemAO_(provisioningAO_.ao, configurationLoader),
          displayAO_(display)
    {
    }

    std::expected<void, SystemOrchestratorErrorStatus> SystemOrchestrator::orchestrate()
    {
        provisioningAO_.start();
        displayAO_.start();

        systemAO_.start();
        return std::expected<void, SystemOrchestratorErrorStatus>();
    }
}