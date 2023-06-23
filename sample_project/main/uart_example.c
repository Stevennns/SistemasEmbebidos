#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define UART_PORT UART_NUM_2
#define UART_TX_PIN 22
#define UART_RX_PIN 21
#define BAUD_RATE 115200

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, 256, 0, 0, NULL, 0);
}

void send_data(const char *data, size_t length) {
    uart_write_bytes(UART_PORT, data, length);
}

void generate_random_data(char *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        data[i] = rand() % 256;
    }
}

void app_main() {
    srand(time(NULL));
    uart_init();

    while (1) {
        char data[10];
        generate_random_data(data, sizeof(data));

        send_data(data, sizeof(data));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}