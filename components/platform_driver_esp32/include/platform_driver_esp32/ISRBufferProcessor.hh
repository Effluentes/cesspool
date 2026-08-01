#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <cstdint>
#include <array>
#include <atomic>

template <typename Derived, typename T, uint8_t CAPACITY = 8>
class ISRBufferProcessor
{
    static_assert(CAPACITY > 0 && (CAPACITY & (CAPACITY - 1)) == 0,
                  "CAPACITY must be power o 2");

public:
    ISRBufferProcessor() noexcept = default;

    void IRAM_ATTR pushFromISR(const T &item) noexcept
    {
        const uint8_t h = head_.load(std::memory_order_relaxed);
        const uint8_t next = (h + 1) & MASK;

        if (next == tail_.load(std::memory_order_acquire))
            return;

        buf_[h] = item;
        head_.store(next, std::memory_order_release);
    }

protected:
    TaskHandle_t taskHandle_{nullptr};
    void startPollTask(const char *name,
                       uint32_t stack = 2048,
                       UBaseType_t prio = 5)
    {
        xTaskCreate(pollTask, name, stack, this, prio, &taskHandle_);
    }

private:
    static void pollTask(void *arg)
    {
        auto *self = static_cast<ISRBufferProcessor *>(arg);
        T sample;
        while (true)
        {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            while (self->pop(sample))
            {
                static_cast<Derived *>(self)->processSample(sample);
            }
        }
    }

    bool pop(T &out) noexcept
    {
        const uint8_t t = tail_.load(std::memory_order_relaxed);
        if (t == head_.load(std::memory_order_acquire))
        {
            return false;
        }
        out = buf_[t];
        tail_.store((t + 1) & MASK, std::memory_order_release);
        return true;
    }

    static constexpr uint8_t MASK = CAPACITY - 1;
    std::array<T, CAPACITY> buf_{};
    std::atomic<uint8_t> head_{0};
    std::atomic<uint8_t> tail_{0};
};