#pragma once
#include <esp_attr.h>
#include "platform_driver_esp32/ISRBufferProcessor.hh"
#include "platform_driver_esp32/EncoderDriverEvent.hh"
#include <driver/gpio.h>

struct EncoderRawSample
{
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t btn : 1;
    uint8_t : 5;
};

class EncoderDriver final
    : public ISRBufferProcessor<EncoderDriver, EncoderRawSample, 16>
{
    friend class ISRBufferProcessor<EncoderDriver, EncoderRawSample, 16>;

public:
    EncoderDriver(gpio_num_t pinA,
                  gpio_num_t pinB,
                  gpio_num_t pinBtn) noexcept;
    void initialize();

private:
    // input::EncoderDebouncer debouncer_{};

    void configureGPIO();
    void applyISRCallback();

    static void onISRCallback(void *arg);
    void handleIsr() noexcept;

    void processSample(const EncoderRawSample &sample) noexcept;
    void publishEvent(EncoderNavDirection dir) noexcept;

    gpio_num_t pinA_;
    gpio_num_t pinB_;
    gpio_num_t pinBtn_;
};