#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO  21        // Pin GPIO para el reloj SCL
#define I2C_MASTER_SDA_IO  22        // Pin GPIO para los datos SDA
#define I2C_MASTER_NUM     I2C_NUM_0 // Puerto I2C utilizado
#define I2C_MASTER_FREQ_HZ 100000    // Velocidad de comunicación I2C

#define BME688_SENSOR_ADDR 0x76       // Dirección I2C del sensor BME688

void i2c_master_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL; // 0
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void app_main()
{
    uint8_t data;
    i2c_master_init();
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME688_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0xD0, true); // Dirección del registro de identificación
    //printf("ret registo = %d\n",ret1);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME688_SENSOR_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    //printf("ret = %d\n",ret);

    printf("data = %d\n", data); // Imprimir el contenido del registro

}