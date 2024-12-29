#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "myOLED.h"
#include "hardware/i2c.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h" //bi_decl
#include <queue.h>


    uint8_t buf[SSD1306_BUF_LEN];

float temperature_value = 30+1;

static QueueHandle_t xQueue = NULL;

int gloal_variable = 100;

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
        //gloal_variable -= 1;
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
        //gloal_variable += 1;
    }
}



void i2c_read_task()
{
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

//     uint uIValueToSend = 0;
//     uIValueToSend = 1;
//     xQueueSend(xQueue, &uIValueToSend, 0U);


    // should printf be used in the task execution?
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
}

void oled_task(){


     //i2c_read_task();
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
        for (int i = 0; i < 2; i += 1) {
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


void oled_task2(){

     //i2c_read_task();
//     SSD1306_init();

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
        for (int i = 0; i < 2; i += 1) {
            for (int x = 3; x < SSD1306_WIDTH; x += 10) {
                DrawLine(buf, x, 0,  SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, pix);
                render(buf, &frame_area);
                vTaskDelay(1);
            }
        WriteFloat(buf, 5, 5, temperature_value, 3);  // Assuming myBuffer is your OLED buffer

            for (int y = SSD1306_HEIGHT-1; y >= 0 ;y-=10) {
                DrawLine(buf, 0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, pix);
                render(buf, &frame_area);
                vTaskDelay(1);
            }
                    WriteFloat(buf, 15, 15, temperature_value, 3);  // Assuming myBuffer is your OLED buffer

            pix = false;
            vTaskDelay(250);
        }
    }
}

void oled_print() {

    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);
    WriteInt(buf, 5, 5, gloal_variable);
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

    i2c_read_task();
    printf("I2C i2c_read_task\n");



    SSD1306_init();
    printf("SSD1306_init\n");


        calc_render_area_buflen(&frame_area);
    //printf("calc_render_area_buflen\n");

    // zero the entire display

    memset(buf, 0, SSD1306_BUF_LEN);
    //printf("memset(buf, 0, SSD1306_BUF_LEN\n");
    render(buf, &frame_area);
    //printf("render(buf, &frame_area\n");

    printf("WriteInt(buf, 5, 5, x111) \n");
        WriteInt(buf, 5, 5, 111);  // Assuming myBuffer is your OLED buffer
    printf("WriteInt(buf, 5, 5, y111) \n");
        render(buf, &frame_area);
    printf("WriteInt(buf, 5, 5, z111) \n");



    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
     xTaskCreate(led_red_task, "LED_Red_Task", 256, NULL, 1, NULL);

     //xTaskCreate(oled_print, "oled_print", 256, NULL, 1, NULL);

//      xTaskCreate(oled_task, "oled_task", 256, NULL, 2, NULL);
//      xTaskCreate(oled_task2, "oled_task2", 256, NULL, 3, NULL);
    vTaskStartScheduler();

    while(1){};
}
