#include "mbed.h"
#include <cstdio>
#include <string>

#define MAXIMUM_BUFFER_SIZE 32

BusInOut rows(PA_1, PA_2, PA_3, PA_4);
BusInOut cols(PB_3, PB_4, PB_5);

static BufferedSerial serial_port(PA_9, PA_10);
FileHandle *mbed::mbed_override_console(int fd)
{ return &serial_port;}


int define_row(int mask) {
    switch (mask) {
        case 0xB: return 1;
        case 0xD: return 2;
        case 0xE: return 3;
        case 0x7: return 4;
    }
    return 0;
}

int define_col(int mask) {
    switch (mask) {
        case 0x1: return 1;
        case 0x2: return 2;
        case 0x4: return 3; 
    }
    return 0;
}

// main() runs in its own thread in the OS
int main()
{
    serial_port.set_baud(9600);
    serial_port.set_format(8, BufferedSerial::None, 1);

    rows.mode(PullUp);
    cols.mode(PullDown);

    int prev_row = 0;
    int prev_col = 0;

    while (true) {
        rows.input();
        cols.output();
        int row = define_row(rows & rows.mask());

        rows.output();
        cols.input();
        int col = define_col(cols & cols.mask()); 
        if (prev_row != row || prev_col != col) {
            prev_col = col;
            prev_row = row;
            printf("%s%d%s%d%c", "Row: ", row, ", col: ", col, '\n');
        }
        ThisThread::sleep_for(500);
    }
}

