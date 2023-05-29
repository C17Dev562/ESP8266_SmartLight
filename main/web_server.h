/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-09-21 20:27:46
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2022-10-16 12:02:07
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\web_server.h
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
// #include "protocol_examples_common.h"
#include "nvs.h"
#include "nvs_flash.h"
// #include "fatfs/vfs/esp_vfs_fat.h"
#include <esp_http_server.h>

#include "define_value.h"
#include "err_mark.h"

static char ssid_buf[33] = {0};
static char passwd_buf[64] = {0};

extern const uint8_t web_file_index_html_start[] asm("_binary_index_html_start");
extern const uint8_t web_file_index_html_end[] asm("_binary_index_html_end");

ymerr start_web_server(wifi_config_t *wifi_config );
