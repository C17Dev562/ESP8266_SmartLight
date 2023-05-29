/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-10-15 13:56:36
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-02-13 16:01:29
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\define_value.h
 * @Description:
 *  该文件存放该项目用到的信息和全局变量
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
#ifndef _DEFINE_VALUE_H_
#define _DEFINE_VALUE_H_

#include <stdio.h>
#include "esp_log.h"

#define def_ap_ssid     "YM-AP"
#define def_ap_passwd   "12345678"

#define NVS_WIFI_SSID "wifissid"
#define NVS_WIFI_PASSWORD "wifipasswd"

#define MAN_IS_NOT_EXIST    0x00
#define MAN_IS_EXIST        0x01

#define MAN_IS_NOT_MOVE     0x01
#define MAN_IS_MOVE         0x02

#define MAN_IS_CLOSING      0x01
#define MAN_IS_FARAWAY      0x02

#define LIGHT_ON    true
#define LIGHT_OFF   false

#define NEED_TO_CHANGE true
#define NOT_NEED_TO_CHANGE false

static const char *TAG = "YM_IOT_1_P: ";

struct light_config_s{
    int lightMode;         //灯光模式
    int nightLightMode;    //夜灯模式
    int nlmStart;         //夜灯模式开始时间
    int nlmEnd;           //夜灯模式结束时间
    int nlmLightStrength;   //夜间亮度数值
    int envLightMode;      //环境光模式
    int envLightStrengthLevel;  //环境光强度
};

int man_exist;
int man_move;
int man_move_to;

bool light_onoff;   //灯光状态
bool light_onoff_needchange;
int lightStrength;  //灯光亮度
bool lightStrength_needchange;

struct light_config_s light_config ;
bool light_config_needchange;



#endif

