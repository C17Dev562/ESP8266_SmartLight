/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-10-15 13:56:36
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-02-13 16:57:16
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\ym_init.c
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
#include "ym_init.h"

extern int man_exist;
extern int man_move;
extern int man_move_to;

extern bool light_onoff;   //灯光状态
extern bool light_onoff_needchange;
extern int lightStrength;  //灯光亮度
extern bool lightStrength_needchange;

extern struct light_config_s light_config ;
extern bool light_config_needchange;

// extern color_rgb_t rgb_temp;
// extern color_hsv_t hsv_temp;

ymerr nvs_init_all(uint8_t control){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND || control == 1) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
        return YM_WARN_NVS_CLEAR;
    }
    ESP_ERROR_CHECK( err );
    return YM_OK;
}

void gValueInit(){
    man_exist = 0;
    man_move = 0;
    man_move_to = 0;
    light_onoff = 0;   //灯光状态
    lightStrength = 0;  //灯光亮度
    light_onoff_needchange = NEED_TO_CHANGE;
    lightStrength_needchange = NEED_TO_CHANGE;
    light_config_needchange = NEED_TO_CHANGE;

    // memset( (void *)&rgb_temp, 0x00, sizeof(color_rgb_t));  //清空

    // rgb_temp.r = 255;   //设定黄色夜光灯的值
    // rgb_temp.g = 198;
    // rgb_temp.b = 107;

    // memset( (void *)&hsv_temp, 0x00, sizeof(color_hsv_t));  //清空
    // memset( (void *)&light_config, 0x00, sizeof(struct light_config_s));    //清空
}

void must_init(){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    gValueInit();
    WS2812_Init();
    setAllPixelColor_nocache(0,0,0);
}

void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "ntp1.aliyun.com");
    sntp_setservername(1, "1.cn.pool.ntp.org");
    sntp_init();
    ESP_LOGI(TAG,"Waitting SYN time , status:%d",sntp_get_sync_status());
    while( sntp_get_sync_status() != 1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG,"Time SYN Completed ");
    sntp_stop();
    setenv("TZ", "CST-8", 1);
    tzset();
}
