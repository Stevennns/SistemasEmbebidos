#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define UART_PORT UART_NUM_2
#define UART_TX_PIN 3
#define UART_RX_PIN 3
#define BAUD_RATE 115200
#define BUF_SIZE (1024)

const char* TAG = "UART";
void uart_init() {
    //printf("inicializado\n");
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB // Añadir esta línea
    };

    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, 256, 0, 0, NULL, 0);
}

void send_data(const char *data, size_t length) {
    //printf("Mandando data");
    uart_write_bytes(UART_PORT, data, length);
}

void generate_random_data(char *data, size_t length) {
    //printf("random data\n");
    /*for (size_t i = 0; i < length; i++) {
        data[i] = rand() % 256;
        //printf("%d",data[i]);
        //ESP_LOGI(TAG, "Data: %d", data[i]);
        
    }*/
    data[0] = 0;
    data[1] = 1;
    data[2] = 2;
    data[3] = 3;
    data[4] = 4;
    data[5] = 5;
    data[6] = 6;
    data[7] = 7;
    data[8] = 8;
    data[9] = 9;
    //printf("aaa");
}


void app_main() {
    //printf("Mandando datos");
    //srand(time(NULL));
    uart_init();

    while (1) {
        char data[10];
        //char data2[128];
        //int len = 0;
        generate_random_data(data, sizeof(data));
        //send_data(data, sizeof(data));
        uart_write_bytes(UART_PORT, &data, sizeof(data));
        //uart_read_bytes(UART_PORT,data2,len,100);
        //for(int i = 0; i<10;i++){
        //    printf("%s",data2);
        //}
        //printf("\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    //xTaskCreate(uart_receive_task, "uart_receive_task", 2048, NULL, 10, NULL);

}