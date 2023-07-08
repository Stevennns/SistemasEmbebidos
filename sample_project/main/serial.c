#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define UART_PORT UART_NUM_0
#define UART_TX_PIN 1
#define UART_RX_PIN 3
#define BAUD_RATE 115200

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, 1024, 0, 0, NULL, 0);
}

void uart_receive() {
    uint8_t data[1024];  // Arreglo para almacenar los datos recibidos

    uart_init();

    while (1) {
        int len = uart_read_bytes(UART_PORT, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
        //printf("Datos recibidos: %s\n", data);
        //printf("len = %d",len);
        if (len > 0) {
            data[len] = '\0';  // Agregar el terminador nulo al final de los datos recibidos
            //printf("Datos recibidos: %s\n", data);

            break;
        }
    }
}

void app_main() {
    uart_receive();
}
