#include "esp32/human_machine_interface/controls/Controls.hh"

#include <array>
#include <cstdint>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <runtime/actors/common/Signals.hh>

namespace
{
    constexpr gpio_num_t encoderClockPin = GPIO_NUM_16;
    constexpr gpio_num_t encoderDataPin = GPIO_NUM_17;
    constexpr gpio_num_t encoderSwitchPin = GPIO_NUM_18;
    constexpr gpio_num_t greenButtonPin = GPIO_NUM_26;
    constexpr gpio_num_t blueButtonPin = GPIO_NUM_27;
    constexpr TickType_t pollingIntervalTicks =
        pdMS_TO_TICKS(10) == 0 ? 1 : pdMS_TO_TICKS(10);
    constexpr TickType_t debounceTicks = pdMS_TO_TICKS(30);
    constexpr std::array<std::int8_t, 16> encoderTransitions{
        0, -1, 1, 0,
        1, 0, 0, -1,
        -1, 0, 0, 1,
        0, 1, -1, 0};

    struct DebouncedButton
    {
        gpio_num_t pin;
        QP::QSignal signal;
        int stableLevel;
        int lastLevel;
        TickType_t lastChange;
    };
}

namespace esp32::human_machine_interface::controls
{
    esp_err_t Controls::start(QP::QActive &systemAO)
    {
        if (started_)
        {
            return ESP_ERR_INVALID_STATE;
        }

        gpio_config_t config{};
        config.pin_bit_mask =
            (1ULL << encoderClockPin) |
            (1ULL << encoderDataPin) |
            (1ULL << encoderSwitchPin) |
            (1ULL << greenButtonPin) |
            (1ULL << blueButtonPin);
        config.mode = GPIO_MODE_INPUT;
        config.pull_up_en = GPIO_PULLUP_ENABLE;
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        config.intr_type = GPIO_INTR_DISABLE;

        const auto configResult = gpio_config(&config);
        if (configResult != ESP_OK)
        {
            return configResult;
        }

        systemAO_ = &systemAO;
        const auto taskResult = xTaskCreate(
            taskEntry,
            "controls",
            3072,
            this,
            5,
            nullptr);
        if (taskResult != pdPASS)
        {
            systemAO_ = nullptr;
            return ESP_ERR_NO_MEM;
        }

        started_ = true;
        return ESP_OK;
    }

    void Controls::taskEntry(void *arg)
    {
        static_cast<Controls *>(arg)->run();
    }

    void Controls::run()
    {
        const auto initialTime = xTaskGetTickCount();
        std::array<DebouncedButton, 3> buttons{{
            {encoderSwitchPin, runtime::actors::CONTROL_SELECT_SIG,
             gpio_get_level(encoderSwitchPin), gpio_get_level(encoderSwitchPin), initialTime},
            {greenButtonPin, runtime::actors::CONTROL_CONFIRM_SIG,
             gpio_get_level(greenButtonPin), gpio_get_level(greenButtonPin), initialTime},
            {blueButtonPin, runtime::actors::CONTROL_BACK_SIG,
             gpio_get_level(blueButtonPin), gpio_get_level(blueButtonPin), initialTime}}};

        auto previousEncoderState = static_cast<std::uint8_t>(
            (gpio_get_level(encoderClockPin) << 1) | gpio_get_level(encoderDataPin));
        std::int8_t encoderPosition = 0;

        for (;;)
        {
            const auto now = xTaskGetTickCount();
            const auto encoderState = static_cast<std::uint8_t>(
                (gpio_get_level(encoderClockPin) << 1) | gpio_get_level(encoderDataPin));
            const auto transitionIndex = static_cast<std::uint8_t>(
                (previousEncoderState << 2) | encoderState);
            encoderPosition += encoderTransitions[transitionIndex];
            previousEncoderState = encoderState;

            if (encoderPosition >= 4)
            {
                postSignal(runtime::actors::CONTROL_ROTATE_RIGHT_SIG);
                encoderPosition = 0;
            }
            else if (encoderPosition <= -4)
            {
                postSignal(runtime::actors::CONTROL_ROTATE_LEFT_SIG);
                encoderPosition = 0;
            }

            for (auto &button : buttons)
            {
                const int level = gpio_get_level(button.pin);
                if (level != button.lastLevel)
                {
                    button.lastLevel = level;
                    button.lastChange = now;
                }
                else if (level != button.stableLevel &&
                         now - button.lastChange >= debounceTicks)
                {
                    button.stableLevel = level;
                    if (level == 0)
                    {
                        postSignal(button.signal);
                    }
                }
            }

            vTaskDelay(pollingIntervalTicks);
        }
    }

    void Controls::postSignal(QP::QSignal signal)
    {
        static QP::QEvt const rotateLeftEvent{runtime::actors::CONTROL_ROTATE_LEFT_SIG, 0, 0};
        static QP::QEvt const rotateRightEvent{runtime::actors::CONTROL_ROTATE_RIGHT_SIG, 0, 0};
        static QP::QEvt const selectEvent{runtime::actors::CONTROL_SELECT_SIG, 0, 0};
        static QP::QEvt const confirmEvent{runtime::actors::CONTROL_CONFIRM_SIG, 0, 0};
        static QP::QEvt const backEvent{runtime::actors::CONTROL_BACK_SIG, 0, 0};

        QP::QEvt const *event = nullptr;
        switch (signal)
        {
        case runtime::actors::CONTROL_ROTATE_LEFT_SIG:
            event = &rotateLeftEvent;
            break;
        case runtime::actors::CONTROL_ROTATE_RIGHT_SIG:
            event = &rotateRightEvent;
            break;
        case runtime::actors::CONTROL_SELECT_SIG:
            event = &selectEvent;
            break;
        case runtime::actors::CONTROL_CONFIRM_SIG:
            event = &confirmEvent;
            break;
        case runtime::actors::CONTROL_BACK_SIG:
            event = &backEvent;
            break;
        default:
            return;
        }

        systemAO_->POST(event, this);
    }
}
