#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <bme680.h>
#include <string.h>


#define PORT 0
#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_0)
#define ADDR 0x76
#endif
#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_1)
#define ADDR BME680_I2C_ADDR_1
#endif

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif
/*
esp_err_t bme680_parallel_measurement(bme680_t *dev)
{
    CHECK_ARG(dev);
    if (dev->meas_started)
    {
        ESP_LOGE(TAG, "Measurement is already running");
        return ESP_ERR_INVALID_STATE;
    }

    // Set the power mode to forced mode to trigger one TPHG measurement cycle
    CHECK_LOGE(bme680_set_mode(dev, BME680_PARALLEL_MODE),
            "Could not set parallel mode to start TPHG measurement cycle");
    dev->meas_started = true;
    dev->meas_status = 0;

    ESP_LOGD(TAG, "Started measurement");

    return ESP_OK;
}
esp_err_t bme680_measure_fixed2(bme680_t *dev, bme680_values_fixed_t *results)
{
    CHECK_ARG(dev && results);

    uint32_t duration;
    CHECK(bme680_get_measurement_duration(dev, &duration));
    if (duration == 0)
    {
        ESP_LOGE(TAG, "Failed to get measurement duration");
        return ESP_FAIL;
    }

    CHECK(bme680_parallel_measurement(dev));
    vTaskDelay(duration);

    return bme680_get_results_fixed(dev, results);
}
*/

void bme680_test(void *pvParameters)
{
    bme680_t sensor;
    memset(&sensor, 0, sizeof(bme680_t));

    ESP_ERROR_CHECK(bme680_init_desc(&sensor, 0x76, PORT, 21, 22));

    // init the sensor
    ESP_ERROR_CHECK(bme680_init_sensor(&sensor));

    // Changes the oversampling rates to 4x oversampling for temperature
    // and 2x oversampling for humidity. Pressure measurement is skipped.
    bme680_set_oversampling_rates(&sensor, BME680_OSR_4X, BME680_OSR_NONE, BME680_OSR_2X);

    // Change the IIR filter size for temperature and pressure to 7.
    bme680_set_filter_size(&sensor, BME680_IIR_SIZE_7);

    // Change the heater profile 0 to 200 degree Celsius for 100 ms.
    bme680_set_heater_profile(&sensor, 0, 200, 100);
    bme680_use_heater_profile(&sensor, 0);

    // Set ambient temperature to 10 degree Celsius
    bme680_set_ambient_temperature(&sensor, 10);

    // as long as sensor configuration isn't changed, duration is constant
    uint32_t duration;
    bme680_get_measurement_duration(&sensor, &duration);

    TickType_t last_wakeup = xTaskGetTickCount();

    bme680_values_float_t values;
    while (1)
    {
        // trigger the sensor to start one TPHG measurement cycle
        /*if (bme680_parallel_measurement(&sensor) == ESP_OK) //(bme680_force_measurement(&sensor) == ESP_OK)
        {
            // passive waiting until measurement results are available
            vTaskDelay(duration);

            // get the results and do something with them
            if (bme680_get_results_float(&sensor, &values) == ESP_OK)
                printf("BME680 Sensor: %.2f °C, %.2f %%, %.2f hPa, %.2f Ohm\n",
                        values.temperature, values.humidity, values.pressure, values.gas_resistance);
        }*/
        if (bme680_force_measurement(&sensor) == ESP_OK) //(bme680_force_measurement(&sensor) == ESP_OK)
        {
            // passive waiting until measurement results are available
            vTaskDelay(duration);

            // get the results and do something with them
            if (bme680_get_results_float(&sensor, &values) == ESP_OK)
                printf("BME680 Sensor: %.2f °C, %.2f %%, %.2f hPa, %.2f Ohm\n",
                        values.temperature, values.humidity, values.pressure, values.gas_resistance);
        }
        // passive waiting until 1 second is over
        vTaskDelayUntil(&last_wakeup, pdMS_TO_TICKS(1000));
    }
}

void read_bme688_data(double array[], bme680_values_float_t values, bme680_t sensor){
   
    if (bme680_get_results_float(&sensor, &values) == ESP_OK){
        array[0] = values.temperature;
        array[1] = values.humidity;
        array[2] = values.pressure;
        array[3] = values.gas_resistance;
    }
}
void app_main3()
{
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreatePinnedToCore(bme680_test, "bme680_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}