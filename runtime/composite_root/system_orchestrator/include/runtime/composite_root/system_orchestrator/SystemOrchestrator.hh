#pragma once
#include <expected>
#include <runtime/composite_root/system_orchestrator/SystemOrchestratorErrorStatus.hh>
#include <runtime/actors/system_ao/SystemAO.hh>
#include <runtime/actors/display_ao/DisplayAO.hh>
#include <array>
#include <utility>
#include <runtime/actors/common/Priority.hh>
#include <configuration/configuration/interface/ConfigurationLoader.hh>
#include <esp32/human_machine_interface/controls/Controls.hh>

namespace runtime::composite_root
{
    class SystemOrchestrator
    {
    public:
        explicit SystemOrchestrator(
            configuration::interface::ConfigurationLoader &configurationLoader,
            human_machine_interface::display::interface::Display &display);
        std::expected<void, SystemOrchestratorErrorStatus> orchestrate();

    private:
        template <typename ActiveObjectType, actors::ActiveObjectPriority priority>
        struct ActiveObject
        {
            using EventQueue = std::array<QP::QEvt const *, 10>;
            static constexpr std::uint_fast16_t taskStackSizeBytes = 4096;

            template <typename... Args>
            explicit ActiveObject(Args &&...args)
                : ao(std::forward<Args>(args)...)
            {
            }

            ActiveObjectType ao;
            EventQueue eventQueue;
            void post(QP::QEvt const &event)
            {
                ao.POST(&event, &ao);
            }

            void start()
            {
                static const auto parameters = nullptr;
                auto **queue_ptr = const_cast<QP::QEvt const **>(eventQueue.data());
                auto queue_size = static_cast<std::uint_fast16_t>(eventQueue.size());
                ao.start(
                    static_cast<std::uint_fast8_t>(priority),
                    queue_ptr,
                    queue_size,
                    nullptr,
                    taskStackSizeBytes,
                    parameters);
            }
        };
        ActiveObject<actors::DisplayAO, actors::ActiveObjectPriority::DISPLAY_AO_PRIORITY> displayAO_;
        ActiveObject<actors::SystemAO, actors::ActiveObjectPriority::SYSTEM_AO_PRIORITY> systemAO_;
        esp32::human_machine_interface::controls::Controls controls_;
    };
}