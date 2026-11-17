#pragma once
#include <expected>
#include <runtime/composite_root/system_orchestrator/SystemOrchestratorErrorStatus.hh>
#include <runtime/actors/system_ao/SystemAO.hh>
#include <runtime/actors/display_ao/DisplayAO.hh>
#include <array>
#include <utility>
#include <runtime/actors/common/Priority.hh>
#include <configuration/configuration/interface/ConfigurationLoader.hh>
#include <runtime/application/Application.hh>
#include <runtime/application/InputPort.hh>
#include <runtime/application/LogPort.hh>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstddef>

namespace runtime::composite_root
{
    class SystemOrchestrator
    {
    public:
        explicit SystemOrchestrator(
            configuration::interface::ConfigurationLoader configurationLoader,
            application::DisplayPort display,
            application::InputPort input,
            application::LogPort log);
        std::expected<void, SystemOrchestratorErrorStatus> orchestrate();

    private:
        template <typename ActiveObjectType, actors::ActiveObjectPriority priority>
        struct ActiveObject
        {
            using EventQueue = std::array<QP::QEvt const *, 10>;
            static constexpr std::size_t taskStackWords = 1024;
            struct TaskStorage
            {
                StaticTask_t controlBlock{};
                std::array<StackType_t, taskStackWords> stack{};
            };
            static_assert(offsetof(TaskStorage, stack) == sizeof(StaticTask_t));

            template <typename... Args>
            explicit ActiveObject(Args &&...args)
                : ao(std::forward<Args>(args)...)
            {
            }

            ActiveObjectType ao;
            EventQueue eventQueue;
            TaskStorage taskStorage;

            void start()
            {
                auto **queue_ptr = const_cast<QP::QEvt const **>(eventQueue.data());
                auto queue_size = static_cast<std::uint_fast16_t>(eventQueue.size());
                ao.start(
                    static_cast<std::uint_fast8_t>(priority),
                    queue_ptr,
                    queue_size,
                    &taskStorage.controlBlock,
                    static_cast<std::uint_fast16_t>(sizeof(taskStorage.stack)),
                    nullptr);
            }
        };
        application::Application application_;
        application::InputPort input_;
        application::LogPort log_;
        ActiveObject<actors::DisplayAO, actors::ActiveObjectPriority::DISPLAY_AO_PRIORITY> displayAO_;
        ActiveObject<actors::SystemAO, actors::ActiveObjectPriority::SYSTEM_AO_PRIORITY> systemAO_;
    };
}
