/*
 * @Author: C17_Dev 1370565947@qq.com
 * @Date: 2022-10-09 10:36:52
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-01-07 17:35:48
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\net_tool.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
// #ifdef _NET_TOOL_C_
#pragma once
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_system.h"

#include "nvs.h"

#include "err_mark.h"

#include "define_value.h"
//字符串结构体，包含字符串头和长度
struct str_struct
{
    char * value;
    int32_t length;
};

/* WiFi操作命令 */
typedef uint32_t wifioperate;
/* Definitions for wifi operations. */
#define WIFI_CONFIG_MAKE 0x10
#define WIFI_CONFIG_CHANGE 0x20

static EventGroupHandle_t s_wifi_event_group;

static EventGroupHandle_t ap_wifi_event_group;

/**
 * 检查wifi设置是否存在
 * @brief:检查wifi设置是否存在
 * @Date: 2022-10-09 20:02:19
 * @description:
 * @param {wifi_config_t} *wifi_config
 * @return ymerr 通用错误类型
 *      YM_ERR_NVS_FAIL     NVS组件故障
 *      YM_ERR_NVS_NOWIFICONFIG NVS无WIFI信息
 *      YM_ERR_NVS_READ_FAIL    NVS读取失败
 *      YM_OK   成功
 */
ymerr wificonfcheck(wifi_config_t *wifi_config);


/**
 * 初始化wifi连接路由器
 * @brief:初始化wifi连接路由器
 * @Date: 2022-10-09 20:02:22
 * @description:
 * @param {wifi_config_t} wifi_config
 * @return ymerr 通用错误类型
 *      YM_OK   成功
 *      YM_ERR_WIFI_STA_CONNECT_FAIL    WIFI连接失败
 *      YM_ERR_WIFI_STA_UNKOWNERROR     WIFI未知错误
 */
ymerr wifi_init_sta(wifi_config_t *wifi_config);


/**
 * 写入wifi设置
 * @brief:写入wifi设置
 * @Date: 2022-10-09 20:02:24
 * @description:
 * @param {wifi_config_t} *wificonfig
 * @param {wifioperate} operation
 * @return ymerr 通用错误类型
 *      YM_ERR_NVS_FAIL     NVS故障
 *      YM_ERR_WIFICONFIG_INVALID   输入的WIFI配置无效
 *      YM_ERR_CONTROL_UNKNOWFCOMM  无效的控制参数
 *      YM_OK   成功
 */
ymerr wificonfwrite(wifi_config_t *wificonfig , wifioperate operation);


/**
 * 打印WIFI信息
 * @brief:打印WIFI信息
 * @Date: 2022-10-15 14:06:53
 * @description:
 * @param {wifi_config_t} *wifi_config
 * @return 无
 */
void print_wifi(wifi_config_t *wifi_config);


/**
 * 打开WIFI热点，进行配网
 * @brief:打开WIFI热点，进行配网
 * @Date: 2022-10-15 16:52:25
 * @description:
 * @param {wifi_config_t} *wifi_config_in
 * @return ymerr 通用错误类型 --->
 *      :YM_ERR_DELATE_WEB_WIFICONFIG   获取wifi信息超时
 *      :YM_WHAT_THE_FUCK               位置错误
 *      :YM_OK                          获取成功
 */
ymerr wifi_apNetsetup_start(wifi_config_t *wifi_config_in);


/**
 * 获取字符串形式的MAC
 * @brief:
 * @Date: 2023-01-07 17:32:55
 * @description:
 * @return uint8_t* 数组
 */
uint8_t* get_wifi_mac_str();
