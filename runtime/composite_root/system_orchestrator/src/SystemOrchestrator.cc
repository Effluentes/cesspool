#include "runtime/composite_root/system_orchestrator/SystemOrchestrator.hh"

#include <esp_log.h>

namespace runtime::composite_root
{
    namespace
    {
        constexpr char logTag[] = "SystemOrchestrator";
    }

    SystemOrchestrator::SystemOrchestrator(
        configuration::interface::ConfigurationLoader &configurationLoader)
        : provisioningAO_(),
          systemAO_(provisioningAO_.ao, configurationLoader)

    {
    }

    std::expected<void, SystemOrchestratorErrorStatus> SystemOrchestrator::orchestrate()
    {
        provisioningAO_.start();

        systemAO_.start();
        return std::expected<void, SystemOrchestratorErrorStatus>();
    }
}