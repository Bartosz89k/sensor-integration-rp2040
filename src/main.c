#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"


void led_task()
{   
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(100);
        gpio_put(LED_PIN, 0);
        vTaskDelay(100);
    }
}

void led_red_task()
{
    const uint LED_PIN = 16;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        if (temperature_value > temperature_threshold) {
            gpio_put(LED_PIN, 1);
            vTaskDelay(10);
        }
        else {

            gpio_put(LED_PIN, 1);
            vTaskDelay(10);
            gpio_put(LED_PIN, 0);
            vTaskDelay(10);
        }
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(led_red_task, "LED_Red_Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}
