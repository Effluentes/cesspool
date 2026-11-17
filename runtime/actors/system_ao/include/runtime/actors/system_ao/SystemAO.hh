#pragma once

#include <qpcpp.hpp>
#include <runtime/actors/display_ao/DisplayAO.hh>
#include <configuration/configuration/interface/ConfigurationLoader.hh>
#include <runtime/application/Application.hh>
#include <runtime/application/InputPort.hh>

namespace runtime::actors
{
    class SystemAO final : public QP::QActive
    {
    public:
        SystemAO(
            application::Application &application,
            DisplayAO &displayAO,
            configuration::interface::ConfigurationLoader configurationLoader) noexcept;

        SystemAO(SystemAO const &) = delete;
        SystemAO &operator=(SystemAO const &) = delete;
        void post(application::UserInput input) noexcept;

    private:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(active);

        void dispatchCommand(application::Command const &command);

        application::Application &application_;
        DisplayAO &displayAO_;
        configuration::interface::ConfigurationLoader configurationLoader_;
        application::CommandId nextCommandId_{1};
    };

} // namespace runtime::actors
