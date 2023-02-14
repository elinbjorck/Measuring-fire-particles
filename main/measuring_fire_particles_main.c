/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"

#include "esp_sds011-1.1.0/src/Sds011.h"

void app_main()
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP8266 chip with %d CPU cores, WiFi, ",
            chip_info.cores);

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    
    int counter = 0;

    gpio_config_t pin_config;
    pin_config.intr_type = GPIO_INTR_DISABLE;
    pin_config.mode = GPIO_MODE_OUTPUT;
    pin_config.pin_bit_mask = (1ULL<<5);
    pin_config.pull_down_en = 0;
    pin_config.pull_up_en = 0;
    gpio_config(&pin_config);

    for (int i = 10; i >= 0; i--) {
        counter++;
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(5, counter%2);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}