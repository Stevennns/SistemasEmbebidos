#include "bme68xLibrary.h"
#include "driver/i2c.h"
#include "esp_err.h"

// Define the constants for I2C communication
#define I2C_PORT_NUM 0
#define I2C_SENSOR_ADDR BME68X_I2C_ADDR_LOW
#define I2C_CLK_SPEED 100000
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// Create a Bme68x object
Bme68x bme;

// Create a bme68xData object
bme68xData sensorData;

// Define a commMux object for I2C communication
commMux commSetup;

// Define a function to initialize the I2C driver
void i2c_init()
{
    // Create an i2c_config_t object
    i2c_config_t conf;
    // Set the mode to master
    conf.mode = I2C_MODE_MASTER;
    // Set the SDA and SCL pins
    conf.sda_io_num = (gpio_num_t)I2C_SDA_PIN;
    conf.scl_io_num = (gpio_num_t)I2C_SCL_PIN;
    // Set the pull-up mode to enable
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    // Set the clock speed
    conf.master.clk_speed = I2C_CLK_SPEED;
    // Configure the port with the parameters
    i2c_param_config((i2c_port_t)I2C_PORT_NUM, &conf);
    // Install the driver with no buffer
    i2c_driver_install((i2c_port_t)I2C_PORT_NUM, conf.mode, 0, 0, 0);
}

// Define a function to read data from the sensor
int8_t commMuxRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    // Create an i2c_cmd_handle_t object
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // Start the transmission
    i2c_master_start(cmd);
    // Write the sensor address and the register address
    i2c_master_write_byte(cmd, (I2C_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    // Start a repeated start
    i2c_master_start(cmd);
    // Write the sensor address with the read bit
    i2c_master_write_byte(cmd, (I2C_SENSOR_ADDR << 1) | I2C_MASTER_READ, true);
    // Read the data bytes with an ack except for the last byte
    for (uint32_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            i2c_master_read_byte(cmd, reg_data[i], I2C_MASTER_NACK);
        }
        else
        {
            i2c_master_read_byte(cmd, reg_data[i], I2C_MASTER_ACK);
        }
    }
    // Stop the transmission
    i2c_master_stop(cmd);
    // Execute the command and check the result
    esp_err_t ret = i2c_master_cmd_begin((i2c_port_t)I2C_PORT_NUM, cmd, 1000 / portTICK_RATE_MS);
    // Delete the command
    i2c_cmd_link_delete(cmd);
    // Return 0 if success or -1 if error
    return ret == ESP_OK ? 0 : -1;
}

// Define a function to write data to the sensor
int8_t commMuxWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    // Create an i2c_cmd_handle_t object
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // Start the transmission
    i2c_master_start(cmd);
    // Write the sensor address and the register address
    i2c_master_write_byte(cmd, (I2C_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    // Write the data bytes
    for (uint32_t i = 0; i < len; i++)
    {
        i2c_master_write_byte(cmd, reg_data[i], true);
    }
    // Stop the transmission
    i2c_master_stop(cmd);
    // Execute the command and check the result
    esp_err_t ret = i2c_master_cmd_begin((i2c_port_t)I2C_PORT_NUM, cmd, 1000 / portTICK_RATE_MS);
    // Delete the command
    i2c_cmd_link_delete(cmd);
    // Return 0 if success or -1 if error
    return ret == ESP_OK ? 0 : -1;
}

// Define a function to delay in milliseconds
void commMuxDelay(uint32_t period)
{
    delay(period);
}

void setup()
{
    // Initialize serial communication
    Serial.begin(115200);

    // Initialize I2C communication
    i2c_init();

    // Set the commMux object parameters
    commSetup.intf = BME68X_I2C_INTF;
    commSetup.i2cAddr = BME68X_I2C_ADDR_LOW; // or BME68X_I2C_ADDR_HIGH

    // Initialize the sensor with the commMux object
    bme.begin(BME68X_I2C_INTF, commMuxRead, commMuxWrite, commMuxDelay, &commSetup);

    // Check the status of the sensor
    if (bme.checkStatus() != BME68X_OK)
    {
        Serial.println("Initializing sensor failed with error " + bme.statusString());
        while (1)
            ;
    }

    // Set the TPH parameters of the sensor
    bme.setTPH();

    // Set the gas configuration of the sensor
    bme.setGasConf();

    // Set the operation mode of the sensor to forced mode
    bme.setOpMode(BME68X_FORCED_MODE);
}

void app_main(void)
{
   // Read the data from the sensor and store it in the sensorData object
   bme.getData(&sensorData);

   // Print the data to the serial monitor
   Serial.print("Temperature: ");
   Serial.print(sensorData.temperature);
   Serial.println(" C");
   
   Serial.print("Pressure: ");
   Serial.print(sensorData.pressure);
   Serial.println(" Pa");
   
   Serial.print("Humidity: ");
   Serial.print(sensorData.humidity);
   Serial.println(" %");
   
   Serial.print("Gas resistance: ");
   Serial.print(sensorData.gasResistance);
   Serial.println(" Ohm");

   //
    // Wait for one second before reading again
    delay(1000);
}
