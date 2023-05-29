/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-10-31 15:44:37
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-02-09 16:31:28
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\base_fun.h
 * @Description:
 *      该文件是定义基础功能的函数，基础功能应该包括
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp8266/gpio_struct.h"
#include "esp8266/spi_struct.h"
#include "esp_system.h"
#include "esp_libc.h"
#include "driver/gpio.h"
#include "driver/spi.h"
#include "string.h"
#include "mqtt_work.h"

//启动UART交互通讯
void start_uart_event();
