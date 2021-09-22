#pragma once
#include <cstdint>
#include "bus.h"
#include "io/io.h"

class MockBus: public Bus {
public:
    MockBus();
    ~MockBus();
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
    void force_write(uint16_t address, uint8_t value);
    IO io;

private:
    uint8_t data[0xFFFF+1];
};
