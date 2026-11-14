#pragma once
#include <expected>
#include <runtime/composite_root/system_orchestrator/SystemOrchestratorErrorStatus.hh>
#include <runtime/actors/system_ao/SystemAO.hh>
#include <array>
#include <utility>
#include <runtime/actors/common/Priority.hh>

namespace runtime::composite_root
{
    class SystemOrchestrator
    {
    public:
        SystemOrchestrator();
        std::expected<void, SystemOrchestratorErrorStatus> orchestrate();

    private:
        template <typename T, actors::ActiveObjectPriority priority>
        struct ActiveObject
        {
            using EventQueue = std::array<QP::QEvt const *, 10>;
            static constexpr std::uint_fast16_t taskStackSizeBytes = 4096;

            template <typename... Args>
            explicit ActiveObject(Args &&...args)
                : ao(std::forward<Args>(args)...)
            {
            }

            T ao;
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

                printf("[DEBUG] Próba uruchomienia AO:\n");
                printf("  -> Priorytet (prio): %u\n", static_cast<unsigned>(priority));
                printf("  -> Rozmiar kolejki (qLen): %u\n", static_cast<unsigned>(queue_size));
                printf("  -> Rozmiar stosu taska (stkSize): %u bajtów\n",
                       static_cast<unsigned>(taskStackSizeBytes));

                ao.start(
                    static_cast<std::uint_fast8_t>(priority),
                    queue_ptr,
                    queue_size,
                    nullptr,
                    taskStackSizeBytes,
                    parameters);

                printf("Active object started with priority %u\n", static_cast<unsigned>(priority));
            }
        };
        ActiveObject<actors::SystemAO, actors::ActiveObjectPriority::SYSTEM_AO_PRIORITY> systemAO_;
    };
}