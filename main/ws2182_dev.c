/*
 * @Author: C17_Dev lijialiangww@163.com
 * @Date: 2022-11-05 18:22:19
 * @LastEditors: C17_Dev lijialiangww@163.com
 * @LastEditTime: 2023-02-13 17:56:01
 * @FilePath: \ESP8266_RTOS_SDK\opt\WIFI_Config\main\ws2182_dev.c
 * @Description:
 *
 *
 * Copyright (c) 2022 by C17_Dev lijialiangww@163.com, All Rights Reserved.
 */
#include "WS2182_dev.h"

#include "esp8266/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/spi.h"
#include <math.h>

uint8_t rBuffer[PIXEL_MAX] = {0};
uint8_t gBuffer[PIXEL_MAX] = {0};
uint8_t bBuffer[PIXEL_MAX] = {0};

color_rgb_t rgb_night = {255,198,107};
color_rgb_t rgb_temp = {0,0,0};
color_hsv_t hsv_temp = {0,0,0};

static LED_status led_status = {0,0,0};

#define GPIO_OUTPUT_SET(gpio_no, bit_value) \
    gpio_output_set(bit_value << gpio_no, ((~bit_value) & 0x01) << gpio_no, 1 << gpio_no, 0)

static void IRAM_ATTR spi_event_callback(int event, void *arg)
{
    switch (event)
    {
    case SPI_INIT_EVENT:
    {
    }
    break;

    case SPI_TRANS_START_EVENT:
    {
    }
    break;

    case SPI_TRANS_DONE_EVENT:
    {
    }
    break;

    case SPI_DEINIT_EVENT:
    {
    }
    break;
    }
}

void ws2812_spi_mode_init(void) // must use the ESP8266 GPIO13 as the hspi pin to drive WS2812B RGB LED!!!
{
    uint8_t x = 0;

    ESP_LOGI("WS2812", "ws2812 init gpio");

    ESP_LOGI("WS2812", "init HSPI");
    spi_config_t spi_config;
    // Load default interface parameters
    // CS_EN:1, MISO_EN:1, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1, BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0
    spi_config.interface.val = SPI_DEFAULT_INTERFACE;
    // Load default interrupt enable
    // TRANS_DONE: true, WRITE_STATUS: false, READ_STATUS: false, WRITE_BUFFER: false, READ_BUFFER: false
    spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
    // Cancel hardware cs
    spi_config.interface.cs_en = 0;
    // MISO pin is used for DC
    spi_config.interface.miso_en = 0;
    // CPOL: 1, CPHA: 1
    spi_config.interface.cpol = 1;
    spi_config.interface.cpha = 1;
    //将 SPI 设置为主模式
    // 8266 只支持半双工
    spi_config.mode = SPI_MASTER_MODE;
    // Set the SPI clock frequency division factor
    spi_config.clk_div = SPI_5MHz_DIV;
    // 注册SPI事件回调函数
    spi_config.event_cb = spi_event_callback;
    spi_init(HSPI_HOST, &spi_config);

    ESP_LOGI("WS2812", "init over");
}

//驱动WS2812 LED灯  单引脚初始化
void WS2812_Init(void)
{
    //初始化 HSPI 作为数据输出引脚
    ws2812_spi_mode_init();

}

void WS2812BSend_24bit(uint8_t R, uint8_t G, uint8_t B)
{
    uint32_t GRB = G << 16 | R << 8 | B;
    uint8_t data_buf[24];
    uint8_t *p_data = data_buf;

    //能用　等待优化!
    uint8_t mask = 0x80;
    uint8_t byte = G;

    uint8_t loop_time = 0x80;

    while (loop_time)
    {
        if (byte & mask)
        {
            *p_data = 0xF0; /*11110000b;*/
        }
        else
        {
            *p_data = 0XC0; /*11000000b;*/
        }
        byte <<= 1;
        loop_time >>= 1;//循环8次
        p_data++;
    }

    loop_time = 0x80;
    byte = R;

    while (loop_time)
    {
        if (byte & mask)
        {
            *p_data = 0xF0; /*11110000b;*/
        }
        else
        {
            *p_data = 0XC0; /*11000000b;*/
        }
        byte <<= 1;
        loop_time >>= 1;//循环8次
        p_data++;
    }

    loop_time = 0x80;
    byte = B;

    while (loop_time)
    {
        if (byte & mask)
        {
            *p_data = 0xF0; /*11110000b;*/
        }
        else
        {
            *p_data = 0XC0; /*11000000b;*/
        }
        byte <<= 1;
        loop_time >>= 1;//循环8次
        p_data++;
    }


    uint32_t *spi_buf = (uint32_t *)data_buf;

    spi_trans_t trans = {0};
    trans.mosi = spi_buf;
    trans.bits.mosi = 24 * 8;
    // ETS_INTR_LOCK();
    spi_trans(HSPI_HOST, &trans);
    // ETS_INTR_UNLOCK();
}

void setAllPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t i = 0;
    for (i = 0; i < PIXEL_MAX; i++)
    {
        rBuffer[i] = 0;
        gBuffer[i] = 0;
        bBuffer[i] = 0;
    }
    for (i = 0; i < PIXEL_MAX; i++)
    {
        rBuffer[i] = r;
        gBuffer[i] = g;
        bBuffer[i] = b;
    }

    for (i = 0; i < PIXEL_MAX; i++)
    {
        WS2812BSend_24bit(rBuffer[i], gBuffer[i], bBuffer[i]);
    }
}

void setAllPixelColor_nocache(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t i = 0;

    led_status.r = r;
    led_status.g = g;
    led_status.b = b;

    for (i = 0; i < PIXEL_MAX; i++)
    {
        WS2812BSend_24bit(r, g, b);
    }
}

void HAL_Delay(int time)
{
    // os_delay_us(time*1000);
    vTaskDelay(time / portTICK_RATE_MS);
}

void hsv_to_rgb(int h,int s,int v,float *R,float *G,float *B)
{
    float C = 0,X = 0,Y = 0,Z = 0;
    int i=0;
    float H=(float)(h);
    float S=(float)(s)/100.0;
    float V=(float)(v)/100.0;
    if(S == 0)
        *R = *G = *B = V;
    else
    {
        H = H/60;
        i = (int)H;
        C = H - i;

        X = V * (1 - S);
        Y = V * (1 - S*C);
        Z = V * (1 - S*(1-C));
        switch(i){
            case 0 : *R = V; *G = Z; *B = X; break;
            case 1 : *R = Y; *G = V; *B = X; break;
            case 2 : *R = X; *G = V; *B = Z; break;
            case 3 : *R = X; *G = Y; *B = V; break;
            case 4 : *R = Z; *G = X; *B = V; break;
            case 5 : *R = V; *G = X; *B = Y; break;
        }
    }
    *R = *R *100;
    *G = *G *100;
    *B = *B *100;
}

void set_night_light(int nlmLightStrength ){
    float r_f,g_f,b_f;
    hsv_to_rgb(nlm_H,nlm_S,nlmLightStrength,&r_f,&g_f,&b_f);
    int r,g,b;
    r = (int) (r_f/100*255) ;
    g = (int) (g_f/100*255) ;
    b = (int) (b_f/100*255) ;
    ESP_LOGI("TEAT","rgb is :%d %d %d",r,g,b);
    setAllPixelColor_nocache(r,g,b);
}
