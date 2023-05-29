#ifndef __BASE_FUN_C_
#define __BASE_FUN_C_

#include "base_fun.h"
#include "define_value.h"
extern int man_exist;
extern int man_move;
extern int man_move_to;

//mqtt process define
extern esp_mqtt_client_handle_t client_g ;


//关于UART的部分

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;

static uart_config_t uart_config_radar = {
        .baud_rate = 74880,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

void radar_data_process(uint8_t *dtmp , size_t data_s){
    ESP_LOGI(TAG,"ddddman_exist : %d",man_exist);
    ESP_LOGI(TAG,"ddddman_move : %d",man_move);
    ESP_LOGI(TAG,"ddddman_move_to : %d",man_move_to);
    // int i;
    ESP_LOGI(TAG,"judge 0");
    uint8_t *dtmp_p = dtmp;
    ESP_LOGI(TAG,"dtmp_p = %hX",*dtmp_p);
    dtmp_p++;
    ESP_LOGI(TAG,"dtmp_p = %hX",*dtmp_p);
    if( *dtmp_p++ == 0x53)
        if( *dtmp_p++ == 0x59 ) //帧头判断
            ESP_LOGI(TAG,"judge 1");
            if( *dtmp_p++ == 0x80 ){
                ESP_LOGI(TAG,"judge 2");
            // switch (*(++dtmp_p)) {
            //     case 0x80:
                        switch (*dtmp_p++) {
                            case 0x01:
                                ESP_LOGI(TAG,"judge 3-1");
                                if( *dtmp_p++ == 0x00  )
                                    if( *dtmp_p++ == 0x01){
                                        mqtt_man_exist_change_pro(man_exist,*dtmp_p);
                                        man_exist = *dtmp_p;
                                        ESP_LOGI(TAG,"pp man_exist : %d",man_exist);
                                        return ;
                                    }
                            case 0x02:
                                if( *dtmp_p++ == 0x00  )
                                    if( *dtmp_p++ == 0x01){
                                        ESP_LOGI(TAG,"judge 3-2");
                                        ESP_LOGI(TAG,"dtmp_p = %hX",*dtmp_p);
                                        if( *dtmp_p == MAN_IS_NOT_EXIST ){    //人不存在
                                            mqtt_man_exist_change_pro(man_exist,MAN_IS_NOT_EXIST);
                                            man_exist = MAN_IS_NOT_EXIST;
                                        }else{      //人存在
                                            mqtt_man_exist_change_pro(man_exist,MAN_IS_EXIST);
                                            man_exist = MAN_IS_EXIST;
                                            man_move = *dtmp_p;
                                        }
                                        ESP_LOGI(TAG,"pp man_move : %d",man_move);
                                        return ;
                                    }
                                break;
                            case 0x03:
                                ESP_LOGI(TAG,"judge 3-3");
                                mqtt_man_exist_change_pro(man_exist,MAN_IS_EXIST);
                                man_exist = MAN_IS_EXIST;
                                break;
                            case 0x0b:
                                ESP_LOGI(TAG,"judge 3-4");
                                if( *dtmp_p++ == 0x00  )
                                    if( *dtmp_p++ == 0x01){
                                        if( *dtmp_p == MAN_IS_NOT_EXIST ){    //人不存在
                                            mqtt_man_exist_change_pro(man_exist,MAN_IS_NOT_EXIST);
                                            man_exist = MAN_IS_NOT_EXIST;
                                        }else{      //人存在
                                            mqtt_man_exist_change_pro(man_exist,MAN_IS_EXIST);
                                            man_exist = MAN_IS_EXIST;
                                            man_move_to = *dtmp_p;
                                        }
                                        ESP_LOGI(TAG,"pp man_move_to : %d",man_move_to);
                                        return ;
                                    }
                        }
            // }
            }
}



//UART 0 的接收处理部分
static void uart_event_task_radar(void *pvParameters)
{
    uart_event_t event;
    uint8_t *dtmp = (uint8_t *) malloc(RD_BUF_SIZE);
    for (;;) {
        // Waiting for UART event.
        if (xQueueReceive(uart0_queue, (void *)&event, (portTickType)portMAX_DELAY)) {
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);

            switch (event.type) {
                // Event of UART receving data
                // We'd better handler data event fast, there would be much more data events than
                // other types of events. If we take too much time on data event, the queue might be full.
                case UART_DATA:
                    ESP_LOGI(TAG, "[UART DATA]: %x", event.size);
                    uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
                    radar_data_process(dtmp,event.size);
                    ESP_LOGI(TAG, "[DATA EVT]:");
                    uart_write_bytes(EX_UART_NUM, (const char *) dtmp, event.size);
                    break;

                // Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart0_queue);
                    break;

                // Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart0_queue);
                    break;

                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;

                // Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;

                // Others
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }

    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}




void start_uart_event()
{
    // Configure parameters of an UART driver,
    // communication pins and install the driver
    // uart_config_t uart_config = {
    //     .baud_rate = 74880,
    //     .data_bits = UART_DATA_8_BITS,
    //     .parity = UART_PARITY_DISABLE,
    //     .stop_bits = UART_STOP_BITS_1,
    //     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    // };
    uart_param_config(EX_UART_NUM, &uart_config_radar);

    // Install UART driver, and get the queue.
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 100, &uart0_queue, 0);

    // Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task_radar, "uart_event_task_radar", 2048, NULL, 12, NULL);
}

//UART 部分结束


//SPI 部分开始

esp_err_t esp8266_spi_send_data(uint32_t *data, uint32_t len)
{
    spi_trans_t trans = {0};
    trans.mosi = data;
    trans.bits.mosi = len * 8;
    return spi_trans(HSPI_HOST, &trans);
}

uint32_t esp8266_spi_read_byte(uint32_t *data, uint32_t len)
{
    spi_trans_t trans;
    uint32_t Rdata = 0;
    memset((void*)&trans, 0x0, sizeof(trans));
    trans.bits.val = 0;
    trans.miso = &Rdata;
    trans.bits.miso = 8 * 1;
    spi_trans(HSPI_HOST, &trans);
    return (uint32_t *) Rdata;
}

esp_err_t esp8266_spi_init(void)
{
    ESP_LOGI(TAG, "init spi");
    spi_config_t spi_config;
    // Load default interface parameters
    // CS_EN:1, MISO_EN:1, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1, BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0
    spi_config.interface.val = SPI_DEFAULT_INTERFACE;

    // Load default interrupt enable
    // TRANS_DONE: true, WRITE_STATUS: false, READ_STATUS: false, WRITE_BUFFER: false, READ_BUFFER: false
    spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;

    spi_config.interface.cs_en = 0;
    spi_config.interface.mosi_en = 1;
    spi_config.interface.miso_en = 1;// 置1,否则miso无法接收数据

    // Set SPI to master mode
    // ESP8266 Only support half-duplex
    spi_config.mode = SPI_MASTER_MODE;
    // Set the SPI clock frequency division factor
    spi_config.clk_div = SPI_5MHz_DIV;
    spi_config.event_cb = NULL;
    return spi_init(HSPI_HOST, &spi_config);
}


//SPI 部分结束


#endif
