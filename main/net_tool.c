/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-10-09 10:36:52
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-01-07 17:37:49
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\net_tool.c
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */

#include "net_tool.h"
#include "define_value.h"
#include "web_server.h"
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static int s_retry_num = 0;
#define EXAMPLE_ESP_MAXIMUM_RETRY  5

uint8_t wifimac[6] = {0};           //wifi mac 存放
uint8_t wifimac_str[13] = {0};     //wifi mac 的str存放

void make_wifi_mac_str(){
    esp_wifi_get_mac(WIFI_IF_STA,wifimac);
    uint8_t temp=0;
    for(uint8_t i=0;i<6;i++){
        temp = wifimac[i]>>4;
        if (temp >= 10){
            wifimac_str[i*2] = temp + 'A' - 10;
        }else{
            wifimac_str[i*2] = temp + '0';
        }

        temp = (wifimac[i] & 0x0F);
        if (temp >= 10){
            wifimac_str[i*2+1] = temp + 'A' - 10;
        }else{
            wifimac_str[i*2+1] = temp + '0';
        }
    }
    wifimac_str[12] = '\0';
}

void print_wifi(wifi_config_t *wifi_config){
    ESP_LOGI(TAG, "WIFI_Print SSID  : %s",(char *)wifi_config->sta.ssid);
    ESP_LOGI(TAG, "WIFI_Print PASSWD: %s",(char *)wifi_config->sta.password);
}

ymerr wificonfcheck(wifi_config_t *wifi_config){

    //变量定义
    esp_err_t err ; //SSID读的错误标识
    esp_err_t err1 ;    //PASSWORD读的错误标识
    nvs_handle_t my_handle;     //nvs储存句柄

    //打开nvs储存句柄
    err = nvs_open("netconfig", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return YM_ERR_NVS_FAIL;
    }

    // 从NVS获得wifi配置信息

        // 可优化 - start - 可被独立出来，作为设置
    size_t required_size_ssid;
    size_t required_size_passwd;

    required_size_ssid = 32;
    required_size_passwd = 64;
        // 可优化 - end
        //这两句获取字符串的长度信息
    err = nvs_get_str(my_handle, NVS_WIFI_SSID, NULL , &required_size_ssid);
    err1 = nvs_get_str(my_handle, NVS_WIFI_PASSWORD, NULL , &required_size_passwd);
        //真正读取字符串
    err = nvs_get_str(my_handle, NVS_WIFI_SSID, (char *) wifi_config->sta.ssid , &required_size_ssid);
    err1 = nvs_get_str(my_handle, NVS_WIFI_PASSWORD, (char *) wifi_config->sta.password, &required_size_passwd);

    nvs_close(my_handle);   //关闭句柄

    ESP_LOGI(TAG, " required_size_ssid = %d",required_size_ssid);
    ESP_LOGI(TAG, " required_size_passwd = %d",required_size_passwd);

    print_wifi(wifi_config);

    //验证读取是否成功
    switch ( err | err1 ) {
        case ESP_OK:    //读取成功,继续执行
            break;
        case ESP_ERR_NVS_NOT_FOUND:     //找不到,视为没有WIFI信息
            // nvs_close(my_handle);
            return YM_ERR_NVS_NOWIFICONFIG ;
            break;
        default :
            // nvs_close(my_handle);
            return YM_ERR_NVS_READ_FAIL;    //异常,视为读取异常
    }

    // switch (err1) {
    //     case ESP_OK:
    //         break;
    //     case ESP_ERR_NVS_NOT_FOUND:
    //         nvs_close(my_handle);
    //         return YM_ERR_NVS_NOWIFICONFIG ;
    //         break;
    //     default :
    //         nvs_close(my_handle);
    //         return YM_ERR_NVS_READ_FAIL;
    // }

    if( required_size_passwd < 8 || required_size_passwd > 64 || required_size_ssid < 1 || required_size_ssid > 32){
        // nvs_close(my_handle);
        return YM_ERR_NVS_NOWIFICONFIG;
    }

    ESP_LOGI(TAG, " wifissid : %s",(char *)wifi_config->sta.ssid);
    ESP_LOGI(TAG, " wifipasswd : %s",(char *)wifi_config->sta.password);
    // nvs_close(my_handle);
    //check the wifi config value
    return YM_OK;
}


ymerr wificonfwrite(wifi_config_t *wificonfig , wifioperate operation){

    //变量定义
    esp_err_t err ; //SSID读的错误标识
    esp_err_t err1 ;    //PASSWORD读的错误标识
    nvs_handle_t my_handle; //nvs储存句柄

    err = nvs_open("netconfig", NVS_READWRITE, &my_handle); //打开nvs储存句柄
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return YM_ERR_NVS_FAIL;
    }

    //初始化一个WIFI配置到NVS
    if( operation == WIFI_CONFIG_MAKE){
        nvs_set_str(my_handle, NVS_WIFI_SSID, "\0");
        nvs_set_str(my_handle, NVS_WIFI_PASSWORD, "\0");
        nvs_close(my_handle);
        return YM_OK;
    }

    //修改一个配置到NVS
    if( operation == WIFI_CONFIG_CHANGE){
        uint8_t wifissid_len = strlen( (char *) wificonfig->sta.ssid);
        uint8_t wifipasswd_len = strlen( (char *) wificonfig->sta.password);

        //检查WIFI配置是否有效
        if(wifissid_len == 0 || wifipasswd_len == 0
            || wifissid_len > 32 || wifipasswd_len > 64 ){
            nvs_close(my_handle);
            return YM_ERR_WIFICONFIG_INVALID;
        }

        err = nvs_set_str(my_handle, NVS_WIFI_SSID, (char *)wificonfig->sta.ssid);
        ESP_LOGI(TAG,"wificonfwrite : wifissid change err: %d",err);
        err = nvs_set_str(my_handle, NVS_WIFI_PASSWORD, (char *)wificonfig->sta.password);
        ESP_LOGI(TAG,"wificonfcheck : wifipasswd change err: %d",err);

        nvs_close(my_handle);
        return YM_OK;
    }

    //未知命令
    nvs_close(my_handle);
    return YM_ERR_CONTROL_UNKNOWFCOMM;
}

static void wifi_sta_event_handle(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    //事件是WIFI事件,且操作为WIFI的开始连接操作
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        //事件是WIFI事件,且操作为WIFI的断连操作
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        //如果没有达到最大重连次数
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            //连接失败,设置标志位,事件等待组开始处理失败标志
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");

    //事件是IP事件,且操作为得到IP的操作
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        //例子中是打印IP地址
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        //连接成功设置标志位,事件等待组开始处理成功标志
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

ymerr wifi_init_sta(wifi_config_t *wifi_config){

    ymerr return_value;

    //创建事件组
    s_wifi_event_group = xEventGroupCreate();
    if( s_wifi_event_group ==NULL ){
        printf("Event group creat fail!\n");
    }

    // tcpip_adapter_init();    //在主函数初始化

    // ESP_ERROR_CHECK(esp_event_loop_create_default());    //ji'c

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_sta_event_handle, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_sta_event_handle, NULL));

    /* Setting a password implies station will connect to all security modes including WEP/WPA.
        * However these modes are deprecated and not advisable to be used. Incase your Access point
        * doesn't support WPA2, these mode can be enabled by commenting below line */

    if (strlen((char *)wifi_config->sta.password)) {
        wifi_config->sta.threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    }

    // ESP_LOGI(TAG, " wifissid : %s",(char *)wifi_config->sta.ssid);
    // ESP_LOGI(TAG, " wifipasswd : %s",(char *)wifi_config->sta.password);

    //when below failed the all program will kill itself
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    ESP_LOGI(TAG, "wifi_init_sta finish waitting.");

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                wifi_config->sta.ssid, wifi_config->sta.password );
        return_value = YM_OK;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                wifi_config->sta.ssid, wifi_config->sta.password );
        return_value = YM_ERR_WIFI_STA_CONNECT_FAIL;
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        return_value = YM_ERR_WIFI_STA_UNKOWNERROR;
    }

    if( return_value != YM_OK){
        esp_wifi_stop();
        esp_wifi_deinit();
    }
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_sta_event_handle));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_sta_event_handle));
    vEventGroupDelete(s_wifi_event_group);

    make_wifi_mac_str();
    return return_value;
}


static void wifi_ap_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

ymerr wifi_apNetsetup_start(wifi_config_t *wifi_config_in)
{
    ymerr return_value;

    // tcpip_adapter_init();
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();    //wifi默认设置
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));   //wifi初始化

    ap_wifi_event_group = xEventGroupCreate();  //wifi连接的事件组
    if( ap_wifi_event_group ==NULL ){
        printf("Event group creat fail!\n");
    }

    //注册事件组
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_ap_event_handler, NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = def_ap_ssid,
            .ssid_len = strlen(def_ap_ssid),    //需要处理:WIFI名字上应该带有本机的部分/全部MAC
            .password = def_ap_passwd,
            .max_connection = 5,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    if (strlen(def_ap_passwd) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }   //如果密码不为空,则将wifi设置成非加密

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             def_ap_ssid, def_ap_passwd);

    return_value = start_web_server(wifi_config_in);

    //撤销WIFI初始化
    esp_wifi_stop();
    esp_wifi_deinit();

    switch(return_value){
        case YM_ERR_DELATE_WEB_WIFICONFIG:return YM_ERR_DELATE_WEB_WIFICONFIG;break;
        case YM_OK : return YM_OK ; break;
        default : return YM_WHAT_THE_FUCK;break;
    }
}




uint8_t* get_wifi_mac_str(){
    return wifimac_str;
}
