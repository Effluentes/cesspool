#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>
#include <initializer_list>
#include <iostream>
#include <map>

void resetAndSetOutputMode(gpio_num_t pin)
{
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

extern "C" void app_main(void)
{
    std::map<std::string, gpio_num_t> leds = {
        {"red",
         GPIO_NUM_3},
        {"green",
         GPIO_NUM_4},
        {"blue",
         GPIO_NUM_5},
        {"warmWhite",
         GPIO_NUM_18},
        {"coldWhite",
         GPIO_NUM_19}};

    for (const auto&led : leds)
    {
        resetAndSetOutputMode(led.second);
    }
    for (;;)
    {
        for (const auto& led : leds)
        {
            std::cout << led.first << std::endl;
            std::cout << "enable" << std::endl;
            gpio_set_level(led.second, 1);
            vTaskDelay(100);
            std::cout << "disable" << std::endl;
            gpio_set_level(led.second, 0);
            vTaskDelay(100);
            std::cout << "----------\n";
        }
    }
}