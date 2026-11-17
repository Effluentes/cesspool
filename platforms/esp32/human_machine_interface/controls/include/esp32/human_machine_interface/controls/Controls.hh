#pragma once

#include <runtime/application/InputPort.hh>

#include <array>
#include <cstddef>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace esp32::human_machine_interface::controls
{
    class Controls final
    {
    public:
        std::expected<void, runtime::application::InputError> start(
            runtime::application::InputSink const &sink);

    private:
        static void taskEntry(void *arg);
        void run();

        runtime::application::InputSink sink_{};
        bool started_{false};
        static constexpr std::size_t taskStackWords = 768;
        StaticTask_t taskControlBlock_{};
        std::array<StackType_t, taskStackWords> taskStack_{};
        std::size_t lowestFreeStackWords_{taskStackWords};
    };
}
