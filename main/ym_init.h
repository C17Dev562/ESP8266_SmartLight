/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-10-09 11:21:51
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-02-11 19:22:00
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\ym_init.h
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
//初始化相关

//hand of nvs
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_netif.h"
#include "err_mark.h"
#include "esp_event.h"
#include "define_value.h"
#include <string.h>
#include "lwip/apps/sntp.h"
#include "ws2182_dev.h"
/**
 * NVS模块初始化
 * @brief: NVS模块初始化
 * @Date: 2022-10-12 18:50:55
 * @description:
 * @return ymerr 通用错误表示
 */
ymerr nvs_init_all(uint8_t control);

/**
 * 必须初始化的部分
 * @brief: 必须初始化的部分
 * @Date: 2022-10-12 18:51:17
 * @description:
 * @return {*}
 */
void must_init();

void initialize_sntp();
