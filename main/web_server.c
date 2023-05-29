/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-10-15 14:33:21
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-01-07 10:53:02
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\web_server.c
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
#include "web_server.h"

static EventGroupHandle_t ap_wifi_event_group_local;

#define WIFI_CONFIG_INPUT BIT1

/* An HTTP GET handler */
esp_err_t hello_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* 读取 URL 查询字符串长度并为长度 + 1 分配内存，
     * 空终止的额外字节 */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param_32[32];
            char param[64];
            /* 从查询字符串中获取期望键的值 */
            if (httpd_query_key_value(buf, "wifissid", param_32, sizeof(param_32)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => wifissid=%s", param_32);
                strcpy(ssid_buf,param_32);
            }
            if (httpd_query_key_value(buf, "wifipasswd", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => wifipasswd=%s", param);
                strcpy(passwd_buf,param);
            }
            xEventGroupSetBits(ap_wifi_event_group_local, WIFI_CONFIG_INPUT);
        }
        free(buf);
    }

    /* 置一些自定义标题 */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* 送带有自定义标头和正文设置为的响应
     * 在用户上下文中传递的字符串*/

    char *str_web =web_file_index_html_start;
    unsigned int str_len =  web_file_index_html_end - web_file_index_html_start;
    httpd_resp_send(req, str_web, str_len);

    /* 发送 HTTP 响应后旧的 HTTP 请求
     * 标头丢失。检查现在是否可以读取 HTTP 请求标头。 */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }

    return ESP_OK;
}

httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // 设置 URI 处理程序
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static httpd_handle_t server = NULL;

ymerr start_web_server(wifi_config_t *wifi_config )
{
    // ESP_ERROR_CHECK(nvs_flash_init());   //集成在ym_init中
    // ESP_ERROR_CHECK(esp_netif_init());   //在主函数初始化
    // ESP_ERROR_CHECK(esp_event_loop_create_default());    //集成在ym_init中

    // ESP_ERROR_CHECK(example_connect());  //初始化网络连接,---未判定---

    ap_wifi_event_group_local = xEventGroupCreate();
    server = start_webserver();

    EventBits_t bits_s = xEventGroupGetBits( ap_wifi_event_group_local );
    EventBits_t bits = bits_s;

    while( ( bits & WIFI_CONFIG_INPUT ) !=  WIFI_CONFIG_INPUT  ){//等待被设置
        bits = xEventGroupWaitBits(ap_wifi_event_group_local,
                WIFI_CONFIG_INPUT,
                pdFALSE,
                pdFALSE,
                portMAX_DELAY);
    }

    stop_webserver(server);

    if( (bits & WIFI_CONFIG_INPUT ) == 0){
        vEventGroupDelete(ap_wifi_event_group_local);
        return YM_ERR_DELATE_WEB_WIFICONFIG;
    }else{
        vEventGroupDelete(ap_wifi_event_group_local);
        strcpy((char*) wifi_config->sta.ssid,ssid_buf);
        strcpy((char*) wifi_config->sta.password,passwd_buf);
        return YM_OK;
    }

}
