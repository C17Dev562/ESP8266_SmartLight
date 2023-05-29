/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-11-05 18:21:52
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-02-13 16:59:25
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\ws2182_dev.h
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
#ifndef __WS2182_DEV__
#define __WS2182_DEV__

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/queue.h"

    #include "driver/gpio.h"
    #include "driver/hw_timer.h"

    #include "esp_log.h"
    #include "esp_system.h"

    #include "esp_libc.h"
    #include "rom/gpio.h"

//当前LED的状态
typedef struct LED_status_s {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}LED_status;

typedef struct color_rgb_t_s
{
    uint8_t  r;
    uint8_t  g;
    uint8_t  b;
}color_rgb_t;

typedef struct color_hsv_t_s
{
    uint8_t h;
    uint8_t  s;
    uint8_t  v;
}color_hsv_t;

// 夜间模式的颜色 - HSV

#define nlm_H   37
#define nlm_S   58
#define nlm_V   100

// 使用的LED灯的个数
#define PIXEL_MAX 32 // 可以多设置,但是不能少设置

//初始化用于驱动WS2812 LED灯的HSPI引脚
void WS2812_Init(void);

//设置所有灯的颜色-暂不适用
void setAllPixelColor(uint8_t r, uint8_t g, uint8_t b);

//设置所有灯的颜色-不经过缓冲
void setAllPixelColor_nocache(uint8_t r, uint8_t g, uint8_t b);

//时间延迟-ms
void HAL_Delay(int time);

void set_night_light(int nlmLightStrength );

#endif
