#include "mbed.h"
#include <chrono>

using namespace std::chrono_literals;
static AnalogIn ain(PA_5);
static BufferedSerial esp_serial(PA_2, PA_3);
static PwmOut led(PB_5);
static BufferedSerial serial_port(PA_9, PA_10);

FileHandle *mbed::mbed_override_console(int fd) { return &serial_port;}

int main()
{
    esp_serial.set_baud(9600);
    esp_serial.set_format(8, BufferedSerial::None, 1);

    serial_port.set_baud(9600);
    serial_port.set_format(8, BufferedSerial::None, 1);

    unsigned char buf[] = {0};

    while (true) {
        esp_serial.read(buf, sizeof(buf));
        printf("%d\n", buf[0]);
        led = buf[0]/255.0;
    }
} 
