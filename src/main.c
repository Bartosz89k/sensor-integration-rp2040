#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "myOLED.h"
#include "hardware/i2c.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h" //bi_decl

/* Allocate heap memory */
// uint8_t ucHeap[configTOTAL_HEAP_SIZE];

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

    // Initialize render area for entire frame (SSD1306_WIDTH pixels by SSD1306_NUM_PAGES pages)
    struct render_area frame_area = {
        start_col: 0,
        end_col : SSD1306_WIDTH - 1,
        start_page : 0,
        end_page : SSD1306_NUM_PAGES - 1
        };


void led_task()
{
    const uint LED_PIN = CYW43_WL_GPIO_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        cyw43_arch_gpio_put(LED_PIN, 1);
        vTaskDelay(100);
        cyw43_arch_gpio_put(LED_PIN, 0);
        vTaskDelay(100);
    }
}

void led_red_task()
{
    const uint LED_PIN = 16;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
            gpio_put(LED_PIN, 1);
            vTaskDelay(10);
            gpio_put(LED_PIN, 0);
            vTaskDelay(10);
    }
}


void oled_task(){


    calc_render_area_buflen(&frame_area);
    //printf("calc_render_area_buflen\n");

    // zero the entire display
    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    //printf("memset(buf, 0, SSD1306_BUF_LEN\n");
    render(buf, &frame_area);
    //printf("render(buf, &frame_area\n");

    WriteInt(buf, 5, 5, 111);  // Assuming myBuffer is your OLED buffer
    render(buf, &frame_area);
    vTaskDelay(50);

    WriteInt(buf, 5, 5, 777);  // Assuming myBuffer is your OLED buffer
    render(buf, &frame_area);
    // intro sequence: flash the screen 3 times
    for (int i = 0; i < 3; i++) {
        SSD1306_send_cmd(SSD1306_SET_ALL_ON);    // Set all pixels on
        vTaskDelay(50);
        SSD1306_send_cmd(SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
        vTaskDelay(50);
    }

    WriteInt(buf, 5, 5, 222);  // Assuming myBuffer is your OLED buffer
    render(buf, &frame_area);
    vTaskDelay(500);

    while(true) {
        bool pix = true;
        for (int i = 0; i < 2; i += 3) {
            for (int x = 0; x < SSD1306_WIDTH; x += 5) {
                DrawLine(buf, x, 0,  SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, pix);
                render(buf, &frame_area);
                vTaskDelay(1);
            }

            for (int y = SSD1306_HEIGHT-1; y >= 0 ;y-=5) {
                DrawLine(buf, 0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, pix);
                render(buf, &frame_area);
                vTaskDelay(1);
            }
            pix = false;
            vTaskDelay(250);
        }
    }
}


int main()
{

    stdio_init_all();
    printf("initialized all!!!\n");
        // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c_default, 100 * 1000);
    printf("I2C initialized\n");

    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
    printf("Wi-Fi init Passed\n");


    cyw43_arch_enable_sta_mode();
    printf("cyw43_arch_enable_sta_mode ???????????????????????\n");


    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
//     xTaskCreate(led_red_task, "LED_Red_Task", 256, NULL, 1, NULL);
//     xTaskCreate(oled_task, "oled_task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}
