#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define PORT_NAME "COM3"  // Replace with the appropriate serial port

int app_main() {
    int fd = open(PORT_NAME, O_WRONLY | O_NOCTTY);
    if (fd == -1) {
        perror("Failed to open the serial port");
        return 1;
    }

    struct termios serial_config;
    if (tcgetattr(fd, &serial_config) != 0) {
        perror("Failed to get serial port settings");
        close(fd);
        return 1;
    }

    // Configure serial port settings
    serial_config.c_iflag = 0;
    serial_config.c_oflag = 0;
    serial_config.c_cflag = CS8 | CREAD | CLOCAL;
    serial_config.c_lflag = 0;
    serial_config.c_cc[VMIN] = 1;
    serial_config.c_cc[VTIME] = 5;
    cfsetispeed(&serial_config, B9600);
    cfsetospeed(&serial_config, B9600);

    if (tcsetattr(fd, TCSANOW, &serial_config) != 0) {
        perror("Failed to apply serial port settings");
        close(fd);
        return 1;
    }

    char data[] = "Hello, world!";  // Data to be written

    ssize_t bytes_written = write(fd, data, strlen(data));
    if (bytes_written == -1) {
        perror("Failed to write data to serial port");
        close(fd);
        return 1;
    }

    printf("Data written successfully: %s\n", data);

    close(fd);
    return 0;
}