#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <iostream>

#define TRIG_PIN GPIO_NUM_2
#define ECHO_PIN GPIO_NUM_1

extern "C" void app_main(void)
{
    std::cout << "start0";
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    std::cout << "start1";

    while (1)
    {
        std::cout<<"start";
        // Send trigger pulse
        gpio_set_level(TRIG_PIN, 0);
        vTaskDelay(2);
        gpio_set_level(TRIG_PIN, 1);
        vTaskDelay(10);
        gpio_set_level(TRIG_PIN, 0);

        // Measure echo pulse width
        // uint64_t start_time = esp_timer_get_time();
        std::cout<<"pierwszy while";
        while (!gpio_get_level(ECHO_PIN))
            ; // Wait for HIGH
        uint64_t echo_start = esp_timer_get_time();
        std::cout<<"drugi while";
        while (gpio_get_level(ECHO_PIN))
            ;
        uint64_t echo_end = esp_timer_get_time();

        uint64_t duration = echo_end - echo_start;
        double distance = ((duration * 348.0) / 2)/10000;

        std::cout<<"Distance:"<<distance<<std::endl;
        vTaskDelay(50);
    }
}