#pragma once
#include <cstdint>
#include "bus.h"
#include "io/io.h"

class MockBus: public Bus {
public:
    MockBus();
    MockBus(bool log_serial);
    ~MockBus();
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
    void force_write(uint16_t address, uint8_t value);
    void load_file(std::string path);
    char *get_serial_data_log();
    int get_serial_data_newline_count();
    IO io;

private:
    uint8_t data[0xFFFF+1];
    bool log_serial;
    char serial_data[50];
    int serial_data_newline_count;
    int serial_data_last_char_index;
};
