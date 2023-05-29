/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-10-09 12:18:12
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2022-10-12 16:56:40
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\err_mark.h
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
//错误类型和标记定义文件

#ifndef err_mark_h

#include <stdio.h>

#endif

//错误常量定义
typedef int32_t ymerr;

#define YM_OK 0x00
#define YM_ERR_CONTROL_UNKNOWFCOMM 0x01

#define YM_ERR_NVS_FAIL 0x100
#define YM_ERR_NVS_NOWIFICONFIG 0x101
#define YM_ERR_NVS_READ_FAIL 0x102
#define YM_WARN_NVS_CLEAR 0x103

#define YM_ERR_WIFICONFIG_INVALID 0x201

#define YM_ERR_WIFI_STA_CONNECT_FAIL 0x220
#define YM_ERR_WIFI_STA_UNKOWNERROR 0x221

#define YM_ERR_DELATE_WEB_WIFICONFIG 0x301

#define YM_WHAT_THE_FUCK 0x999
