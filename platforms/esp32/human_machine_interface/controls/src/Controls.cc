#include "esp32/human_machine_interface/controls/Controls.hh"

#include <array>
#include <cstdint>
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_task_wdt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace
{
    constexpr char tag[] = "Controls";
    using runtime::application::InputError;
    using runtime::application::UserInput;

    constexpr gpio_num_t encoderClockPin = GPIO_NUM_16;
    constexpr gpio_num_t encoderDataPin = GPIO_NUM_17;
    constexpr gpio_num_t encoderSwitchPin = GPIO_NUM_18;
    constexpr gpio_num_t greenButtonPin = GPIO_NUM_6;
    constexpr gpio_num_t blueButtonPin = GPIO_NUM_7;
    constexpr TickType_t pollingIntervalTicks =
        pdMS_TO_TICKS(10) == 0 ? 1 : pdMS_TO_TICKS(10);
    constexpr TickType_t debounceTicks =
        pdMS_TO_TICKS(30) == 0 ? 1 : pdMS_TO_TICKS(30);
    constexpr std::array<std::int8_t, 16> encoderTransitions{
        0, -1, 1, 0,
        1, 0, 0, -1,
        -1, 0, 0, 1,
        0, 1, -1, 0};

    struct DebouncedButton
    {
        gpio_num_t pin;
        UserInput input;
        int stableLevel;
        int lastLevel;
        TickType_t lastChange;
    };
}

namespace esp32::human_machine_interface::controls
{
    std::expected<void, runtime::application::InputError> Controls::start(
        runtime::application::InputSink const &sink)
    {
        if (started_)
        {
            return std::unexpected(InputError::resourceUnavailable);
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

        if (gpio_config(&config) != ESP_OK)
        {
            return std::unexpected(InputError::hardwareInitializationFailed);
        }

        sink_ = sink;
        if (xTaskCreateStatic(
                taskEntry,
                "controls",
                static_cast<uint32_t>(taskStack_.size()),
                this,
                5,
                taskStack_.data(),
                &taskControlBlock_) == nullptr)
        {
            sink_ = {};
            return std::unexpected(InputError::resourceUnavailable);
        }

        started_ = true;
        return {};
    }

    void Controls::taskEntry(void *arg)
    {
        static_cast<Controls *>(arg)->run();
    }

    void Controls::run()
    {
        ESP_ERROR_CHECK(esp_task_wdt_add(nullptr));
        const auto initialTime = xTaskGetTickCount();
        std::array<DebouncedButton, 3> buttons{{
            {encoderSwitchPin, UserInput::select,
             gpio_get_level(encoderSwitchPin), gpio_get_level(encoderSwitchPin), initialTime},
            {greenButtonPin, UserInput::confirm,
             gpio_get_level(greenButtonPin), gpio_get_level(greenButtonPin), initialTime},
            {blueButtonPin, UserInput::back,
             gpio_get_level(blueButtonPin), gpio_get_level(blueButtonPin), initialTime}}};

        auto previousEncoderState = static_cast<std::uint8_t>(
            (gpio_get_level(encoderClockPin) << 1) | gpio_get_level(encoderDataPin));
        std::int8_t encoderPosition = 0;

        for (;;)
        {
            ESP_ERROR_CHECK(esp_task_wdt_reset());
            const auto freeStackWords = uxTaskGetStackHighWaterMark(nullptr);
            if (freeStackWords < lowestFreeStackWords_)
            {
                lowestFreeStackWords_ = freeStackWords;
                ESP_LOGI(tag, "Stack low-water mark: %u words",
                         static_cast<unsigned>(lowestFreeStackWords_));
            }

            const auto now = xTaskGetTickCount();
            const auto encoderState = static_cast<std::uint8_t>(
                (gpio_get_level(encoderClockPin) << 1) | gpio_get_level(encoderDataPin));
            const auto transitionIndex = static_cast<std::uint8_t>(
                (previousEncoderState << 2) | encoderState);
            encoderPosition += encoderTransitions[transitionIndex];
            previousEncoderState = encoderState;

            if (encoderPosition >= 4)
            {
                sink_.post(UserInput::rotateRight);
                encoderPosition = 0;
            }
            else if (encoderPosition <= -4)
            {
                sink_.post(UserInput::rotateLeft);
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
                        sink_.post(button.input);
                    }
                }
            }

            vTaskDelay(pollingIntervalTicks);
        }
    }
}
