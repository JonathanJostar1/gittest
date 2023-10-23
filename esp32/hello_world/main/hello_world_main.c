/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "ringbuf.h"
#include "string.h"


TaskHandle_t task1_handle;
const static char *TAG = "hello_world";

static ringbuf_t rb_handle;


static void task_1(void *args)
{
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_LOGI(TAG, "hello_world");
    }

}

void app_main(void)
{
    // xTaskCreate(task_1, "task_1", 2048, NULL, 2, &task1_handle);
    int ret;
    char write_data[64] = {0};
    ret = ringbuf_init(&rb_handle, 32);
    snprintf(write_data, sizeof(write_data), "The FreeRTOS 202212.00 release updates FreeRTOS Kernel,");


    ret = ringbuf_write(&rb_handle, (uint8_t *)write_data, strlen(write_data));
    ESP_LOGI(TAG, "ret: %d, unuse len: %d, write len: %d", ret, ringbuf_unwrite_byte(&rb_handle), ringbuf_unread_byte(&rb_handle));

    ESP_LOGI(TAG, "state: %d", ringbuf_state_get(&rb_handle));

    char read_data[64] = {0};
    ret = ringbuf_read(&rb_handle, (uint8_t *)read_data, ringbuf_unread_byte(&rb_handle));
    ESP_LOGI(TAG, "state: %d, %s", ringbuf_state_get(&rb_handle), read_data);
    ESP_LOGI(TAG, "unuse len: %d, write len: %d", ringbuf_unwrite_byte(&rb_handle), ringbuf_unread_byte(&rb_handle));

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
