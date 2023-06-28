#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define BUF_SIZE (1024)

void uart_receive_task(void *pvParameters)
{
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    while (1)
    {
        // Leer datos de la UART
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE - 1, 100 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            // Decodificar y procesar los datos recibidos
            data[len] = '\0'; // Agregar el terminador de cadena
            printf("Datos recibidos: %s\n", data);
        }
    }
    free(data);
    vTaskDelete(NULL);
}

void app_main()
{
    // Configurar la UART
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB}; // Añadir esta línea}
    uart_param_config(UART_NUM_1, &uart_config);

    // Configurar los pines UART
    uart_set_pin(UART_NUM_1, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Instalar el controlador UART
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Crear tarea para recibir datos de UART
    xTaskCreate(uart_receive_task, "uart_receive_task", 2048, NULL, 10, NULL);
}
