#include "event_pro.h"

extern int man_exist;
extern int man_move;
extern int man_move_to;

extern bool light_onoff; // 灯光状态
extern bool light_onoff_needchange;
extern int lightStrength; // 灯光亮度
extern bool lightStrength_needchange;

extern struct light_config_s light_config;
extern bool light_config_needchange;

TimerHandle_t statusUR_Handle;



void status_update_report(TimerHandle_t logOP_handle)
{
    ESP_LOGI(TAG, "statusUpRep_Timer-----Run");
    // if (light_onoff_needchange == NEED_TO_CHANGE)
    // {
        // light_onoff_needchange = NOT_NEED_TO_CHANGE; // 清除标注位
        // if (light_onoff == LIGHT_OFF)
        // {
        //     setAllPixelColor_nocache(0, 0, 0);
        //     return;
        // }
    // }

    // if (light_config_needchange == NEED_TO_CHANGE)
    // {
    //     light_onoff_needchange = NOT_NEED_TO_CHANGE; // 清除标注位

    if (light_config.nightLightMode == 1)
    {
        // 获取当前时间
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);
        if (timeinfo.tm_hour >= light_config.nlmStart && timeinfo.tm_hour <= light_config.nlmEnd)
        { // 判定是否为夜间
            ESP_LOGI(TAG, "statusUpRep_Timer111");
            set_night_light( light_config.nlmLightStrength);
            return;
        }
    }
    // }

    //不是处于夜间模式
    int lightness = 0;
    lightness = (int)(lightStrength/100.0*255*(int)light_onoff);
    setAllPixelColor_nocache( lightness , lightness , lightness );
}

void start_event_list()
{
    statusUR_Handle = xTimerCreate("statusUpRep_Timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, status_update_report);
    xTimerStart(statusUR_Handle, pdMS_TO_TICKS(1000));
}
