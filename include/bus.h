#pragma once
#include <cstdint>

class Bus {
public:
    Bus();
    ~Bus();
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
// private:
};
