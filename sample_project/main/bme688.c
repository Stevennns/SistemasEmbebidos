#include "driver/i2c.h"
#include "bme68x/bme68x.h"
#include <string.h>
#define I2C_MASTER_SDA_IO 21 // select SDA GPIO specific to your project
#define I2C_MASTER_SCL_IO 22 // select SCL GPIO specific to your project
//#define I2C_MASTER_FREQ_HZ 100000 // select frequency specific to your project
#define I2C_MASTER_FREQ_HZ 400000 // select frequency specific to your project

#define BME680_ADDR 0x61 // address of the BME680 sensor

volatile uint8_t i2c_data[32]; // volatile variable to store the data






// User-defined function to read from the sensor
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    int8_t rslt = 0;
    //uint8_t dev_id = *((uint8_t *)intf_ptr);
    uint8_t dev_id = *((uint8_t *) intf_ptr);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_id << 1) | I2C_MASTER_WRITE, true); // write the device address and write bit
    i2c_master_write_byte(cmd, reg_addr, true); // write the register address
    i2c_master_write_byte(cmd, (dev_id << 1) | I2C_MASTER_READ, true); // write the device address and read bit
    if (len > 1) {
        i2c_master_read(cmd, reg_data, len - 1, I2C_MASTER_ACK); // read the data and send ACK
    }
    i2c_master_read_byte(cmd, reg_data + len - 1, I2C_MASTER_NACK); // read the last byte and send NACK
    i2c_master_stop(cmd);
    rslt = i2c_master_cmd_begin(0, cmd, 2000 / portTICK_PERIOD_MS); // send the command
    printf("rslt read = %d\n", rslt); // print the result
    i2c_cmd_link_delete(cmd);
    
    return rslt;
}


// User-defined function to write to the sensor
int8_t user_i2c_write(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) // Cambiar el tipo de reg_data a uint8_t
{
    printf("USER_I2C_WRITE LLAMADO\n");
    int8_t rslt = 0;
    printf("reg_addr = %d\n", reg_addr); printf("reg_data = %hhn\n", reg_data);
    printf("reg_data (p) = %p\n", reg_data); printf("reg_data (hhn)= %hhn\n", reg_data);
    for (int i = 0; i < len; i++) { printf("reg_data[%d] = %d\n", i, reg_data[i]); }
    //for (int i = 0; i < len; i++) { printf("reg_addres[%d] = %d\n", i, reg_addr[i]); }
    printf("AASSDadsdsa\n");
    printf("intf_ptr (p) = %p\n", intf_ptr);
    //for (int i = 0; i < len; i++) { printf("intf_ptr[%d] = %d\n", i, intf_ptr[i]); }
    uint8_t dev_id = *((uint8_t *)intf_ptr);
    //uint8_t dev_id = (uint8_t *) intf_ptr;
    printf("dev_id = %d\n", dev_id);
    printf("antes de cmd link create");
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    printf("antes de start\n");
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_id << 1) | I2C_MASTER_WRITE, true);
    printf("antes de reg_addrs\n");
    i2c_master_write_byte(cmd, reg_addr, true);
    vTaskDelay(2 / portTICK_PERIOD_MS); // Esperar 2 ms para que el sensor esté listo
    printf("antes de reg_data\n");
    i2c_master_write(cmd, reg_data, len, true); // Cambiar el tipo de reg_data a uint8_t
    vTaskDelay(2 / portTICK_PERIOD_MS); // Esperar 2 ms para que el sensor esté listo

    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 2000 / portTICK_PERIOD_MS);
    if (ret == ESP_OK) {
        rslt = 0;
    } else {
        rslt = 1;
    }
    i2c_cmd_link_delete(cmd);
    printf("rslt write = %d\n", rslt); // print the result
    return rslt;

}





// User-defined function to delay in microseconds
void user_delay_us(uint32_t period, void *intf_ptr)
{
    esp_rom_delay_us(period);
}


// Initialize the I2C driver and configure the pins
void i2c_master_init()
{
    printf("Inicializando init \n");
    int i2c_master_port = 0; // use port 0
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .clk_flags = 0,
    };
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

void i2c_scanner()
{
    int32_t i2c_err;
    uint8_t address;
    printf("Scanning I2C bus...\n");
    for (address = 1; address < 127; address++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        i2c_err = i2c_master_cmd_begin(0, cmd, 2000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        if (i2c_err == ESP_OK) {
            printf("I2C device found at address 0x%02x\n", address);
        }
    }
    printf("Scanning done.\n");
}

// Read data from the BME680 sensor
void read_bme688_data()
{
    printf("Leyenndo datos\n");
    int8_t rslt;
    struct bme68x_dev dev;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;
    struct bme68x_data data[3];

    dev.intf = BME68X_I2C_INTF;
    uint8_t b = 0x76; // una variable local 
    void* intf_ptr = &b; // asignamos al puntero la dirección de b
    dev.chip_id = BME68X_CHIP_ID;
    dev.intf_ptr = intf_ptr; 
    dev.read = (bme68x_read_fptr_t)user_i2c_read; // user-defined function to read from the sensor
    dev.write = (bme68x_write_fptr_t)user_i2c_write; // user-defined function to write to the sensor
    dev.delay_us = (bme68x_delay_us_fptr_t)user_delay_us; // user-defined function to delay in microseconds
    printf("Despues de la estructura\n"); // print the result
    //rslt = BME68X_OK;
    rslt = bme68x_init(&dev); // initialize the device
    printf("llamado bme68x_init(&dev)\n"); // print the result
    vTaskDelay(5 / portTICK_PERIOD_MS); // Esperar 5 ms para que el sensor esté listo
    if (rslt != BME68X_OK) {
        printf("Failed to initialize the device\n");
        return;
    }

    rslt = bme68x_get_conf(&conf, &dev); // get the default configuration
    printf("llamado bme68x_get_conf\n"); // print the result
    if (rslt != BME68X_OK) {
        printf("Failed to get configuration\n");
        return;
    }

    conf.filter = BME68X_FILTER_OFF; // set the filter to off
    conf.odr = BME68X_ODR_NONE; // set the output data rate to none

    rslt = bme68x_set_conf(&conf, &dev); // set the configuration
    printf("llamado bme68x_set_conf\n"); // print the result
    vTaskDelay(5 / portTICK_PERIOD_MS); // Esperar 5 ms para que el sensor esté listo
    if (rslt != BME68X_OK) {
        printf("Failed to set configuration\n");
        return;
    }

    heatr_conf.enable = BME68X_ENABLE; // enable the heater
    heatr_conf.heatr_temp = 320; // set the heater temperature to 320 degrees Celsius
    heatr_conf.heatr_dur = 150; // set the heater duration to 150 milliseconds

    rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &dev); // set the heater configuration
    printf("llamado bme68x_set_heatr_conf\n"); // print the result
    vTaskDelay(5 / portTICK_PERIOD_MS); // Esperar 5 ms para que el sensor esté listo
    if (rslt != BME68X_OK) {
        printf("Failed to set heater configuration\n");
        return;
    }

    rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &dev); // set the operation mode to forced
    printf("llamado bme68x_set_op_mode\n"); // print the result
    vTaskDelay(5 / portTICK_PERIOD_MS); // Esperar 5 ms para que el sensor esté listo
    if (rslt != BME68X_OK) {
        printf("Failed to set operation mode\n");
        return;
    }

    uint8_t n_fields;
    rslt = bme68x_get_data(BME68X_FORCED_MODE, data, &n_fields, &dev); // get the sensor data
    printf("llamado bme68x_get_data\n"); // print the result
    vTaskDelay(5 / portTICK_PERIOD_MS); // Esperar 5 ms para que el sensor esté listo
    if (rslt != BME68X_OK) {
        printf("Failed to get sensor data\n");
        return;
    }

    for (uint8_t i = 0; i < n_fields; i++) {
        printf("Temperature: %0.2f C\n", data[i].temperature / 100.0);
        printf("Pressure: %0.2f Pa\n", data[i].pressure);
        printf("Humidity: %0.2f %%\n", data[i].humidity / 1000.0);
        printf("Gas resistance: %f ohms\n", data[i].gas_resistance);
        printf("Gas index: %d\n", data[i].gas_index);
        printf("Status: %d\n", data[i].status);
    }
}



void app_main()
{
    i2c_master_init(); // initialize the I2C driver
    vTaskDelay(10 / portTICK_PERIOD_MS); // Esperar 10 ms para que el sensor se inicie
    i2c_scanner();
    i2c_driver_delete(0); // delete the I2C driver
    i2c_master_init(); // initialize the I2C driver again
    vTaskDelay(10 / portTICK_PERIOD_MS); // Esperar 10 ms para que el sensor se inicie
    printf("Delay aaaa\n");
    vTaskDelay(2000 / portTICK_PERIOD_MS); // wait for 2 seconds
    read_bme688_data(); // read data from the BME680 sensor
}
