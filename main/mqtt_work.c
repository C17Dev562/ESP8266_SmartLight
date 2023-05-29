/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-11-06 17:37:01
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-03-17 01:47:51
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\mqtt_work.c
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */


#include "mqtt_work.h"
// ROUTE String Info

    //MQTT Topic SUB

char mqtt_sub_topic_head[128] = "/dev/aiot/s/el1/";
char mqtt_pub_topic_head[128] = "/dev/aiot/d/el1/";
char mqtt_topic_temp[128] = {0};
char mqtt_data_temp[256] = {0};
uint16_t topic_rec[16] = {0};

const char ENV_LIST[4][A_ENV_LEN] = {
    A_ENV_dev_str,
    A_ENV_test_str,
    A_ENV_pre_str,
    A_ENV_prod_str
};

const char NAMESPACE_LIST[1][B_NAMESPACE_LEN] = {
    B_NAMESPACE_aiot_str
};

const char SENDP_LIST[2][C_SENDP_LEN] = {
    C_SENDP_server_str,
    C_SENDP_device_str
};

const char MODEL_LIST[1][10] = {
    D_MODEL_el1_str
};

const char ROUTE_LIST[5][ROUTE_LEN] = {
    ROUTE_heartbeat_str,
    ROUTE_config_str,
    ROUTE_do_str,
    ROUTE_info_str,
    ROUTE_event_str
};

const char ROUTE_config_LIST[3][ROUTE_config_LEN] = {
    ROUTE_config_1_heartbeatGap_str,
    ROUTE_config_1_light_str,
    ROUTE_config_1_fail_str
};

const char ROUTE_do_LIST[2][ROUTE_do_LEN] = {
    ROUTE_do_1_light_str,
    ROUTE_do_1_lightStrength_str
};

const char ROUTE_info_LIST[3][ROUTE_info_LEN] = {
    ROUTE_info_1_light_str,
    ROUTE_info_1_lightStrength_str,
    ROUTE_info_1_exist_str
};

const char ROUTE_event_LIST[1][ROUTE_event_LEN] = {
    ROUTE_event_1_existChange_str
};


// Get The Extren Value

extern int man_exist;
extern int man_move;
extern int man_move_to;

extern bool light_onoff;   //灯光状态
extern bool light_onoff_needchange;
extern int lightStrength;  //灯光亮度
extern bool lightStrength_needchange;

extern struct light_config_s light_config;
extern bool light_config_needchange;

uint8_t* wifi_mac_g;

TimerHandle_t mqtt_start_timer_g = NULL;

esp_mqtt_client_handle_t client_g ;

//根据macid生成发布以及订阅的主题头部
void topic_maker(){
    strcat(mqtt_sub_topic_head,(char *)wifi_mac_g);
    strcat(mqtt_pub_topic_head,(char *)wifi_mac_g);
}

//
void mqtt_man_exist_change_pro(int man_exist_old,int man_exist_in ){
    struct timeval ty_i = {0};
    gettimeofday(&ty_i,NULL);
    if(man_exist_old != man_exist_in){
        cJSON* cjson_test = NULL;
        cjson_test = cJSON_CreateObject();
        cJSON_AddNumberToObject(cjson_test, "exist" , man_exist_in );
        cJSON_AddNumberToObject(cjson_test, "time" , 6666 );
        char* json_str = (char *) cJSON_Print(cjson_test);
        esp_mqtt_client_publish(client_g, stradd_copy(mqtt_topic_temp,mqtt_pub_topic_head,"/event/existChange"), json_str , strlen(json_str)+1 , 0, 0);
        cJSON_Delete(cjson_test);
    }
}

//mqtt连接事件触发的操作函数
void mqtt_connect_action(esp_mqtt_client_handle_t client){
    int msg_id;

    msg_id = esp_mqtt_client_subscribe(client_g, stradd_copy(mqtt_topic_temp,mqtt_sub_topic_head,"/do/light"), 0);
    ESP_LOGI(TAG, "MQTT CONNECT ACTION sent subscribe successful /do/light, msg_id=%d", msg_id);
    msg_id = esp_mqtt_client_subscribe(client_g, stradd_copy(mqtt_topic_temp,mqtt_sub_topic_head,"/do/lightStrength"), 0);
    ESP_LOGI(TAG, "MQTT CONNECT ACTION sent subscribe successful /do/lightStrength, msg_id=%d", msg_id);
    msg_id = esp_mqtt_client_subscribe(client_g, stradd_copy(mqtt_topic_temp,mqtt_sub_topic_head,"/config/light"), 0);
    ESP_LOGI(TAG, "MQTT CONNECT ACTION sent subscribe successful /config/light, msg_id=%d", msg_id);
    msg_id = esp_mqtt_client_subscribe(client_g, stradd_copy(mqtt_topic_temp,mqtt_sub_topic_head,"/info/light"), 0);
    ESP_LOGI(TAG, "MQTT CONNECT ACTION sent subscribe successful /info/light, msg_id=%d", msg_id);
    msg_id = esp_mqtt_client_subscribe(client_g, stradd_copy(mqtt_topic_temp,mqtt_sub_topic_head,"/info/lightStrength"), 0);
    ESP_LOGI(TAG, "MQTT CONNECT ACTION sent subscribe successful /info/lightStrength, msg_id=%d", msg_id);
    msg_id = esp_mqtt_client_subscribe(client_g, stradd_copy(mqtt_topic_temp,mqtt_sub_topic_head,"/info/exist"), 0);
    ESP_LOGI(TAG, "MQTT CONNECT ACTION sent subscribe successful /info/exist, msg_id=%d", msg_id);
}

ymerr mqtt_data_action( uint16_t* topic_rec , char* data , int data_len ){
    cJSON *pJsonRoot = NULL;
    cJSON *object = NULL;
    int num = 0;

    ESP_LOGI(TAG,"topic_rec[5][6][7] =\\%d\\%d\\%d\\",topic_rec[5],topic_rec[6],topic_rec[7]);
    switch (topic_rec[5]) {
        case ROUTE_config:
            switch (topic_rec[6]) {
                case ROUTE_config_1_heartbeatGap:
                    return YM_OK;   //Nothing to do
                case ROUTE_config_1_light:
                    pJsonRoot = cJSON_Parse(data);
                    if (pJsonRoot != NULL){
                        object = cJSON_GetObjectItem(pJsonRoot,"lightMode");
                        if( object != NULL && object->type == cJSON_Number){
                            light_config.lightMode = object->valueint;
                            ESP_LOGI(TAG, "MQTT DATA Get <lightMode> , data=%d, out=%d\\",object->valueint,light_config.lightMode);
                        }
                        object = cJSON_GetObjectItem(pJsonRoot,"nightLightMode");
                        if( object != NULL && object->type == cJSON_Number){
                            light_config.nightLightMode = object->valueint;
                            ESP_LOGI(TAG, "MQTT DATA Get <nightLightMode> , data=%d, out=%d\\",object->valueint,light_config.nightLightMode);
                        }
                        object = cJSON_GetObjectItem(pJsonRoot,"nlmStart");
                        if( object != NULL && object->type == cJSON_Number){
                            light_config.nlmStart = object->valueint;
                            ESP_LOGI(TAG, "MQTT DATA Get <nlmStart> , data=%d, out=%d\\",object->valueint,light_config.nlmStart );
                        }
                        object = cJSON_GetObjectItem(pJsonRoot,"nlmEnd");
                        if( object != NULL && object->type == cJSON_Number){
                            light_config.nlmEnd = object->valueint;
                            ESP_LOGI(TAG, "MQTT DATA Get <nlmEnd> , data=%d, out=%d\\",object->valueint,light_config.nlmEnd);
                        }
                        object = cJSON_GetObjectItem(pJsonRoot,"nlmLightStrength");
                        if( object != NULL && object->type == cJSON_Number){
                            light_config.nlmLightStrength = object->valueint;
                            ESP_LOGI(TAG, "MQTT DATA Get <nlmLightStrength> , data=%d, out=%d\\",object->valueint,light_config.nlmLightStrength);
                        }
                        object = cJSON_GetObjectItem(pJsonRoot,"envLightMode");
                        if( object != NULL && object->type == cJSON_Number){
                            light_config.envLightMode = object->valueint;
                            ESP_LOGI(TAG, "MQTT DATA Get <envLightMode> , data=%d, out=%d\\",object->valueint,light_config.envLightMode);
                        }
                        object = cJSON_GetObjectItem(pJsonRoot,"envLightStrengthLevel");
                        if( object != NULL && object->type == cJSON_Number){
                            light_config.envLightStrengthLevel = object->valueint;
                            ESP_LOGI(TAG, "MQTT DATA Get <envLightStrengthLevel> , data=%d, out=%d\\",object->valueint,light_config.envLightStrengthLevel);
                        }
                        cJSON_Delete(pJsonRoot);
                    }
                    break;
                case ROUTE_config_1_fail:
                    return YM_OK;   //Nothing to do
            }
            break;
        case ROUTE_do:
            switch (topic_rec[6]) {
                case ROUTE_do_1_light:
                    ESP_LOGI(TAG, "MQTT DATA Get <do_1_light> , data=%s\\", data );
                    if( data[0] == '0' ){
                        light_onoff = 0;
                        light_onoff_needchange = NEED_TO_CHANGE;
                        ESP_LOGI(TAG, "MQTT DATA Set <light> , value=%d\\", light_onoff );
                    }
                    if( data[0] == '1'){
                        light_onoff = 1;
                        light_onoff_needchange = NEED_TO_CHANGE;
                        ESP_LOGI(TAG, "MQTT DATA Set <light> , value=%d\\", light_onoff );
                    }
                    return YM_OK;
                case ROUTE_do_1_lightStrength:
                    ESP_LOGI(TAG, "MQTT DATA Get <lightStrength> , data=%s\\", data );
                    num = atoi(data);
                    if( num != 0 ){
                        lightStrength = num;
                        lightStrength_needchange = NEED_TO_CHANGE;
                        ESP_LOGI(TAG, "MQTT DATA Set <lightStrength> , value=%d\\", lightStrength );
                        return YM_OK;
                    }
                    return YM_WHAT_THE_FUCK;
            }
        case ROUTE_info:
            switch (topic_rec[6]) {
                char numToString[5] = {0};
                case ROUTE_info_1_light:
                    __itoa(light_onoff,numToString,10);   //转化数字
                    esp_mqtt_client_publish(client_g, stradd_copy(mqtt_topic_temp,mqtt_pub_topic_head,"/info/light"), numToString , strlen(numToString)+1 , 0, 0);
                case ROUTE_info_1_lightStrength:
                    __itoa(lightStrength,numToString,10);   //转化数字
                    esp_mqtt_client_publish(client_g, stradd_copy(mqtt_topic_temp,mqtt_pub_topic_head,"/info/lightStrength"), numToString , strlen(numToString)+1 , 0, 0);
                case ROUTE_info_1_exist:
                    __itoa(man_exist,numToString,10);   //转化数字
                    esp_mqtt_client_publish(client_g, stradd_copy(mqtt_topic_temp,mqtt_pub_topic_head,"/info/exist"), numToString , strlen(numToString)+1 , 0, 0);
            }
            return YM_OK;
    }
    return YM_WHAT_THE_FUCK;
}


static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client_g = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            topic_maker();
            mqtt_connect_action(client_g);
            ESP_LOGI(TAG, "MQTT CONNECT ACTION OVER");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            // ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);

            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            // ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);

            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);

            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_DATA_EVENT:%d",event->msg_id);
            strncpy(mqtt_topic_temp , event->topic , event->topic_len);
            mqtt_topic_temp[event->topic_len] = '\0';
            strncpy(mqtt_data_temp , event->data , event->data_len);
            mqtt_data_temp[event->data_len] = '\0';
            ESP_LOGI(TAG, "MQTT_topic:%s",mqtt_topic_temp);
            ESP_LOGI(TAG, "MQTT_data:%s",mqtt_data_temp);
            if ( mqtt_topic_res(mqtt_topic_temp,event->topic_len,topic_rec) == YM_OK ){
                mqtt_data_action(topic_rec,mqtt_data_temp,event->data_len);
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}




esp_mqtt_client_handle_t mqtt_app_start( uint8_t* wifi_mac )
{
    wifi_mac_g = wifi_mac;
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://8.129.27.176:23073",
        .username = "root",
        .password = "ymi@666",
    };
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128) {
            int c = fgetc(stdin);
            if (c == '\n') {
                line[count] = '\0';
                break;
            } else if (c > 0 && c < 127) {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.uri = line;
        printf("Broker url: %s\n", line);
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    client_g = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client_g, ESP_EVENT_ANY_ID, mqtt_event_handler, client_g);
    esp_mqtt_client_start(client_g);
    return client_g;
}






//----topic处理部分----

ymerr mqtt_topic_res(char *topic, int len, /* uint8_t* type_list , */ uint16_t *content)
{

    int n = 0, p = 0, clen = 0, type = 0; // the len of the type_list , the point of the *topic , the len of every content
    char *topic_head = topic;             // the head of topic
    char *headOfLastW = topic;            // the head of the world
    uint8_t charlen = sizeof(char);
    if (*topic == '/')
    {
        topic += charlen; //剔除多余主题
    }
    while (p < len)
    {
        if ( n <= 5 )
        {
            topic = ( strchr(topic, '/') - charlen );
            clen = topic - headOfLastW; // get the len of the content
            type = topic_get_head(topic, headOfLastW, n);
            content[n] = type;

            topic += charlen + charlen;
            headOfLastW = topic;
            p += clen + 1;
            n++;
        }else{
            topic = ( strchr(topic, '\0' ) - charlen );
            clen = topic - headOfLastW; // get the len of the content

            type = topic_get_route(topic, headOfLastW, content[n-1]);
            content[n] = type;

            topic += charlen + charlen;
            headOfLastW = topic;
            p += clen + 1;
            n++;
            break;
        }
    }

    if (n <= 5)
    {
        return YM_WHAT_THE_FUCK;
    }

    return YM_OK;
}


int topic_get_head(char *topic, char *headOfLastW, int n)
{

    int type = -1, p = 0;
    char tmp[20] = {0};
    strncpy(tmp, headOfLastW, (topic - headOfLastW ) / sizeof(char) + 1);
    switch (n)
    {
    case 0:
        type = (strstr(ENV_LIST, tmp) - (char*)ENV_LIST) / (A_ENV_LEN * sizeof(char)) + A_ENV + 1;
        break;
    case 1:
        type = (strstr(NAMESPACE_LIST, tmp) - (char*)NAMESPACE_LIST) / (B_NAMESPACE_LEN * sizeof(char)) + B_NAMESPACE + 1;
        break;
    case 2:
        type = (strstr(SENDP_LIST, tmp) - (char*)SENDP_LIST) / (C_SENDP_LEN * sizeof(char)) + C_SENDP + 1;
        break;
    case 3:
        type = (strstr(MODEL_LIST, tmp) - (char*)MODEL_LIST) / (D_MODEL_LEN * sizeof(char)) + D_MODEL + 1;
        break;
    case 4:
        type = E_MACID;
        break;
    case 5:
        // int a = strstr(ROUTE_LIST, tmp);
        type = ( (strstr(ROUTE_LIST, tmp) - (char*)ROUTE_LIST) / (ROUTE_LEN * sizeof(char)) +1 ) * 20 + ROUTE;
        break;
    }

    return type;
}

int topic_get_route(char *topic, char *headOfLastW, int route_root)
{
    int type = -1, p = 0;
    char tmp[20] = {0};
    strncpy(tmp, headOfLastW, (topic - headOfLastW) / sizeof(char) + 1);
    switch (route_root)
    {
    case ROUTE_config:
        type = (strstr(ROUTE_config_LIST, tmp) - (char*)ROUTE_config_LIST) / (ROUTE_config_LEN * sizeof(char)) + ROUTE_config + 1;
        break;
    case ROUTE_do:
        type = (strstr(ROUTE_do_LIST, tmp) - (char*)ROUTE_do_LIST) / (ROUTE_do_LEN * sizeof(char)) + ROUTE_do + 1;
        break;
    case ROUTE_info:
        type = (strstr(ROUTE_info_LIST, tmp) - (char*)ROUTE_info_LIST) / (ROUTE_info_LEN * sizeof(char)) + ROUTE_info + 1;
        break;
    case ROUTE_event:
        type = (strstr(ROUTE_event_LIST, tmp) - (char*)ROUTE_event_LIST) / (ROUTE_event_LEN * sizeof(char)) + ROUTE_event + 1;
        break;
    }

    return type;
}
