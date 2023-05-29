/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-10-09 10:36:52
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-05-10 00:07:42
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\main_app.c
 * @Description:
 *      主程序函数
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */

#include "net_tool.h"
#include "ym_init.h"
#include "jsmn.h"
#include "driver/uart.h"
#include "base_fun.h"
#include "define_value.h"
#include "mqtt_work.h"
#include "event_pro.h"

#include <time.h>

#define WIFI_IS_GOOD false
#define WIFI_NOT_GOOD true

wifi_config_t wifi_config = {
        .sta = {
            .ssid = {0},
            .password = {0}
        },
    };    // WIFI接入配置结构体



void vCallback_logOutPut( TimerHandle_t  logOP_handle ){

    ESP_LOGI(TAG,"man_exist : %d",man_exist);
    ESP_LOGI(TAG,"man_move : %d",man_move);
    ESP_LOGI(TAG,"man_move_to : %d",man_move_to);
}

void app_main(void){
    uint8_t* wifi_mac = NULL;
    // uart_enable_swap();
    ymerr err_ym; //收集自定义的错误
    // 初始化
    nvs_init_all(0);
    must_init();

    //被注释
    bool control = WIFI_IS_GOOD; //主流程控制变量
    uint8_t loop_time = 0;

    // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    //开始循环
    while(1){
        err_ym = wificonfcheck( &wifi_config );

        //上一次连接失败，重置WIFI接入配置，重新配网
        if( control == WIFI_NOT_GOOD ){
            wificonfwrite(NULL,WIFI_CONFIG_MAKE);
            err_ym = YM_ERR_NVS_NOWIFICONFIG;
        }

        //在串口输出WIFI信息
        print_wifi(&wifi_config);

        //如果没有找到wifi接入配置，就配置wifi
        if(err_ym != YM_OK ){
            ESP_LOGI(TAG, "NVS_FAIL, Start NetWork Setup");
            err_ym = wifi_apNetsetup_start( &wifi_config );
            //配网失败->重配
            if(err_ym != YM_OK){
                ESP_LOGI(TAG, "WIFI Setup Fail,retry");
                control = WIFI_NOT_GOOD;
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }
            err_ym = wificonfwrite( &wifi_config, WIFI_CONFIG_CHANGE);
            esp_restart();
        }

        printf("wifi STA connect start");
        //在串口输出WIFI信息
        print_wifi(&wifi_config);

        //连接WIFI
        err_ym = wifi_init_sta( &wifi_config );
        if(err_ym == YM_OK){
            break;
        }
        control = WIFI_NOT_GOOD;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "wifi connection over");

    wifi_mac = get_wifi_mac_str();

    initialize_sntp();

    //time test start

        // time_t now;
        // struct tm timeinfo;
        // char strftime_buf[64];
        // time(&now);
        // localtime_r(&now, &timeinfo);
        // setenv("TZ", "CST-8", 1);
        // tzset();
        // if (timeinfo.tm_year < (2016 - 1900)) {
        //     ESP_LOGE(TAG, "The current date/time error");
        // } else {
        //     strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        //     ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
        // }

    //time test end

    //uart test start

    start_uart_event();

    //uart test end

    // mqtt_start
    mqtt_app_start(wifi_mac);

    //process timer
    start_event_list();

}


