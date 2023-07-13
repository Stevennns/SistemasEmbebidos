#include "bmi270.c"
//#include "bme688.c"
#include "uart.c"
//#define BUF_SIZE 16
#include "bme_example.c"

void init_bme688( bme680_t sensor,uint32_t duration){
    
    //printf("Llamado a init_bme688\n");
    //bme680_t sensor;
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
    //uint32_t duration;
    bme680_get_measurement_duration(&sensor, &duration);

    //TickType_t last_wakeup = xTaskGetTickCount();

    //bme680_values_float_t values;

}

void setup_bmi270(){
    ESP_ERROR_CHECK(bmi_init());
    softreset();
    chipid();
    initialization();
    check_initialization();
    performance_powermode();
    powermode();
    internal_status();    
    srand(time(NULL));  // Initialize random seed
}

void app_main(){
    bme680_t sensor;
    bme680_values_float_t values;
    uint32_t duration = 0;

    uart_setup(); // Uart setup
    //printf("se setupeo\n") ;
    // Waiting for an BEGIN to initialize data sending
    char dataResponse1[6];
    //char defaultResponse[7] = "SSSSSS\0";
    int conf[4];
    int OPTION = 0;

    while (1)
    {
        //strcpy(dataResponse1, defaultResponse);
        // ((uint8_t*)buffer)

        int rLen = serial_read(dataResponse1, 6);
        //printf("rLen: %d\n", rLen);
        //printf("dataResponse1: %c\n", dataResponse1);
   
        if (rLen > 0)
        {
            if (strcmp(dataResponse1, "BEGIN") == 0)  // BMI688
            {
                OPTION = 0;
                setup_bmi270();
                //uart_write_bytes(UART_NUM,"OK\0",3);
                receive_conf(conf); // se recibe el arreglo de configuracion
                /*for (int i = 0; i < 4; i++) {
                    printf("Received conf: %d\r\n", conf[i]);
                }*/
                acc_conf(conf[0]); // se configura el acelerometro
                acc_range(conf[1]);
                gyr_conf(conf[2]); // se configura el giroscopio
                gyr_range(conf[3]);

                break; 
            }
            else if (strcmp(dataResponse1, "BEGIM") == 0)
            {OPTION = 1;
                //printf("Llamado a BME688\n");
                init_bme688(sensor,duration);

                break;
                //printf("me equivoque de lugar\n");
                
                
            }

        }
    }

    // Data sending, can be stopped receiving an END between sendings
    char dataResponse2[4];
    //printf("Beginning sending... \n");
    while (1)
    {
        if (OPTION){
            printf("Llamado a if OPTION\n");
            double data[4];
            read_bme688_data(data,values,sensor);
            printf("Temperature: %f, Pressure: %f, Humidity: %f, Gas: %f\n", data[0], data[1], data[2], data[3]);
            serial_write((const char*)data, sizeof(data));

        }
        else{

            double data[9];
            read_bmi270_data(data);
            
            /*const char* dataToSend = (const char*)data;
            int len = strlen(dataToSend);
            serial_write(dataToSend, sizeof(dataToSend));*/
            serial_write((const char*)data, sizeof(data));
        }

        int rLen = serial_read(dataResponse2, 4);
        if (rLen > 0){
            if (strcmp(dataResponse2, "END") == 0){
                break;
            }
        }
        
    }

    // Data sending stopped, sending just OK with \0 at the end
    /*while (1)
    {
        uart_write_bytes(UART_NUM,"OK\0",3);
    }*/

}