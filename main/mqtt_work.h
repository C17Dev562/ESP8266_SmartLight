/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-11-06 17:37:19
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-02-14 19:39:50
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\mqtt_work.h
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
#ifndef MQTT_WORK
#define MQTT_WORK

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
// #include "esp_wifi.h"
#include "esp_system.h"
// #include "nvs_flash.h"
#include "esp_event.h"
// #include "esp_netif.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "err_mark.h"

#include <cJSON.h>
//project source
#include "define_value.h"
#include "math.h"

#include "fun_tool.h"

//MQTT Topic SUB

// char mqtt_sub_topic_head[128] = "/dev/aiot/s/el1/";
// char mqtt_pub_topic_head[128] = "/dev/aiot/d/el1/";


//MQTT Topic define start
#define A_ENV 0
#define A_ENV_dev 1
#define A_ENV_test 2
#define A_ENV_pre 3
#define A_ENV_prod 4

#define A_ENV_dev_str   "dev**"
#define A_ENV_test_str  "test*"
#define A_ENV_pre_str   "pre**"
#define A_ENV_prod_str  "prod*"

#define A_ENV_LEN 5 //所有字段的最大长度，包括结束符

// const char ENV_LIST[4][A_ENV_LEN] = {
//     A_ENV_dev_str,
//     A_ENV_test_str,
//     A_ENV_pre_str,
//     A_ENV_prod_str
// };

    //---
#define B_NAMESPACE 10
#define B_NAMESPACE_aiot 11

#define B_NAMESPACE_aiot_str    "aiot**"

#define B_NAMESPACE_LEN 5   //所有字段的最大长度，包括结束符
// const char NAMESPACE_LIST[1][B_NAMESPACE_LEN] = {
//     B_NAMESPACE_aiot_str
// };
    //---
#define C_SENDP 20
#define C_SENDP_server 21
#define C_SENDP_device 22

#define C_SENDP_server_str  "server****"
#define C_SENDP_device_str  "device****"

#define C_SENDP_LEN 10   //所有字段的最大长度，包括结束符
// const char SENDP_LIST[2][C_SENDP_LEN] = {
//     C_SENDP_server_str,
//     C_SENDP_device_str
// };
    //--
#define D_MODEL 30
#define D_MODEL_el1 31

#define D_MODEL_el1_str     "el1*******"

#define D_MODEL_LEN 10   //所有字段的最大长度，包括结束符
// const char MODEL_LIST[1][10] = {
//     D_MODEL_el1_str
// };
    //--
#define E_MACID 41

    //--
#define ROUTE 90
#define ROUTE_heartbeat 110
#define ROUTE_config 130
#define ROUTE_do 150
#define ROUTE_info 170
#define ROUTE_event 190

#define ROUTE_heartbeat_str "heartbeat*"
#define ROUTE_config_str    "config****"
#define ROUTE_do_str        "do********"
#define ROUTE_info_str      "info******"
#define ROUTE_event_str     "event*****"

#define ROUTE_LEN 10   //所有字段的最大长度，包括结束符
// const char ROUTE_LIST[5][ROUTE_LEN] = {
//     ROUTE_heartbeat_str,
//     ROUTE_config_str,
//     ROUTE_do_str,
//     ROUTE_info_str,
//     ROUTE_event_str
// };

    //--
#define ROUTE_config_1_heartbeatGap 131
#define ROUTE_config_1_light 132
#define ROUTE_config_1_fail 133

#define ROUTE_config_1_heartbeatGap_str "heartbeatGap***"
#define ROUTE_config_1_light_str        "light**********"
#define ROUTE_config_1_fail_str         "fail***********"

#define ROUTE_config_LEN 15  //所有字段的最大长度，包括结束符
// const char ROUTE_config_LIST[3][ROUTE_config_LEN] = {
//     ROUTE_config_1_heartbeatGap_str,
//     ROUTE_config_1_light_str,
//     ROUTE_config_1_fail_str
// };
    //--

#define ROUTE_do_1_light 151
#define ROUTE_do_1_lightStrength 152

#define ROUTE_do_1_light_str            "light**********"
#define ROUTE_do_1_lightStrength_str    "lightStrength**"

#define ROUTE_do_LEN 15   //所有字段的最大长度，包括结束符
// const char ROUTE_do_LIST[2][ROUTE_do_LEN] = {
//     ROUTE_do_1_light_str,
//     ROUTE_do_1_lightStrength_str
// };
    //--

#define ROUTE_info_1_light 171
#define ROUTE_info_1_lightStrength 172
#define ROUTE_info_1_exist 173

#define ROUTE_info_1_light_str          "light**********"
#define ROUTE_info_1_lightStrength_str  "lightStrength**"
#define ROUTE_info_1_exist_str          "exist**********"

#define ROUTE_info_LEN 15   //所有字段的最大长度，包括结束符
// const char ROUTE_info_LIST[3][ROUTE_info_LEN] = {
//     ROUTE_info_1_light_str,
//     ROUTE_info_1_lightStrength_str,
//     ROUTE_info_1_exist_str
// };
    //--

#define ROUTE_event_1_existChange 191

#define ROUTE_event_1_existChange_str   "existChange****"

#define ROUTE_event_LEN 15   //所有字段的最大长度，包括结束符
// const char ROUTE_event_LIST[1][ROUTE_event_LEN] = {
//     ROUTE_event_1_existChange_str
// };
    //--

//MQTT Topic define end

int topic_get_route(char *topic, char *headOfLastW, int route_root);
int topic_get_head(char *topic, char *headOfLastW, int n);

ymerr mqtt_topic_res(char *topic, int len, /* uint8_t* type_list , */ uint16_t *content);

esp_mqtt_client_handle_t mqtt_app_start( uint8_t* wifi_mac );

// static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);

// ymerr mqtt_data_action( uint16_t* topic_rec , char* data , int data_len );

// void mqtt_connect_action(esp_mqtt_client_handle_t client);

// void topic_maker();

#endif
