#pragma once
#include <expected>
#include <runtime/composite_root/system_orchestrator/SystemOrchestratorErrorStatus.hh>
#include <runtime/actors/system_ao/SystemAO.hh>
#include <array>
#include <cstddef>
#include <utility>
#include <runtime/actors/common/Priority.hh>
#include <configuration/configuration/interface/ConfigurationLoader.hh>
#include <runtime/actors/provisioning_ao/ProvisioningAO.hh>

namespace runtime::composite_root
{
    class SystemOrchestrator
    {
    public:
        explicit SystemOrchestrator(
            configuration::interface::ConfigurationLoader &configurationLoader);
        std::expected<void, SystemOrchestratorErrorStatus> orchestrate();

    private:
        template <typename ActiveObjectType, actors::ActiveObjectPriority priority>
        struct ActiveObject
        {
            using EventQueue = std::array<QP::QEvt const *, 10>;
            static constexpr std::uint_fast16_t taskStackSizeBytes = 4096;

            struct TaskStorage
            {
                StaticTask_t taskControlBlock{};
                std::array<StackType_t, taskStackSizeBytes / sizeof(StackType_t)> stack{};
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
                    &taskStorage,
                    sizeof(taskStorage.stack),
                    parameters);
            }
        };
        ActiveObject<actors::SystemAO, actors::ActiveObjectPriority::SYSTEM_AO_PRIORITY> systemAO_;
        ActiveObject<actors::ProvisioningAO, actors::ActiveObjectPriority::PROVISIONING_AO_PRIORITY> provisioningAO_;
    };
}