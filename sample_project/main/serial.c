#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define UART_PORT UART_NUM_2
#define BAUD_RATE 115200

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, 1024, 0, 0, NULL, 0);
}

void uart_send(const char* data) {
    uart_write_bytes(UART_PORT, data, strlen(data));
}

void uart_receive() {
    uint8_t* data = (uint8_t*) malloc(1024);
    while (1) {
        int len = uart_read_bytes(UART_PORT, data, 1024, 1000 / portTICK_PERIOD_MS);
        printf("se recibieron: %d\n",len);
        if (len > 0) {
            data[len] = '\0';
            printf("Datos recibidos: %s\n", data);

            // Procesar los datos recibidos y enviar la respuesta
            if (strcmp((char*)data, "PING") == 0) {
                uart_send("PONG");
            } else {
                uart_send("Comando no reconocido");
            }

            break;
        }
    }
    free(data);
}

void app_main() {
    uart_init();

    uart_receive();
}
