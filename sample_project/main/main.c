#include "bmi270.c"
//#include "bme688.c"
#include "uart.c"
//#define BUF_SIZE 16
void app_main(){
    uart_setup(); // Uart setup
    ESP_ERROR_CHECK(bmi_init());
    softreset();
    chipid();
    initialization();
    check_initialization();
    
    acc_conf(8);
    acc_range(0);

    gyr_conf(12);
    gyr_range(0);
    

    performance_powermode();
    powermode();

    internal_status();    



    srand(time(NULL));  // Initialize random seed


    
    // Waiting for an BEGIN to initialize data sending
    char dataResponse1[6];
    int conf[4];
    //printf("Beginning initialization... \n");
    while (1)
    {
        int rLen = serial_read(dataResponse1, 6);
        if (rLen > 0)
        {
            if (strcmp(dataResponse1, "BEGIN") == 0)
            {
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
        }
    }

    // Data sending, can be stopped receiving an END between sendings
    char dataResponse2[4];
    //printf("Beginning sending... \n");
    while (1)
    {

        double data[9];
        read_bmi270_data(data);
        
        /*const char* dataToSend = (const char*)data;
        int len = strlen(dataToSend);
        serial_write(dataToSend, sizeof(dataToSend));*/
        serial_write((const char*)data, sizeof(data));

        int rLen = serial_read(dataResponse2, 4);
        if (rLen > 0)
        {
            if (strcmp(dataResponse2, "END") == 0)
            {
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