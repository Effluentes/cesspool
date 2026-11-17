#include "runtime/composite_root/system_orchestrator/SystemOrchestrator.hh"

namespace runtime::composite_root
{
    SystemOrchestrator::SystemOrchestrator(
        configuration::interface::ConfigurationLoader configurationLoader,
        application::DisplayPort display,
        application::InputPort input,
        application::LogPort log)
        : input_(input),
          log_(log),
          displayAO_(application_, display),
          systemAO_(application_, displayAO_.ao, configurationLoader)
    {
    }

    std::expected<void, SystemOrchestratorErrorStatus> SystemOrchestrator::orchestrate()
    {
        displayAO_.start();
        systemAO_.start();

        const auto sink = application::InputSink::bind(systemAO_.ao);
        const auto inputResult = input_.start(sink);
        if (!inputResult)
        {
            log_.write(application::LogLevel::error, "Failed to start input controls");
            return std::unexpected(SystemOrchestratorErrorStatus::input_initialization_failed);
        }

        log_.write(application::LogLevel::info, "Application actors started");
        return std::expected<void, SystemOrchestratorErrorStatus>();
    }
}
