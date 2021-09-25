#pragma once
#include "bus.h"

class BusWrapper: public Bus {
public:
    BusWrapper(bool log_serial);
    ~BusWrapper();
    void write(uint16_t address, uint8_t value);
    char *get_serial_data_log();
    int get_serial_data_newline_count();

private:
    bool log_serial;
    char serial_data[50];
    int serial_data_newline_count;
    int serial_data_last_char_index;
};
