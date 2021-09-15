#pragma once
#include <cstdint>

class ReadWriteInterface {
public:
    virtual void write(uint16_t address, uint8_t value) = 0;
    virtual uint8_t read(uint16_t address) = 0;
};
