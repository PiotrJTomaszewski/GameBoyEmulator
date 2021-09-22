#pragma once

#include <cstdint>
#include <string>
#include "read_write_interface.h"

class Cartridge: public ReadWriteInterface {
public:
    Cartridge();
    ~Cartridge();
    void load_file(std::string path);
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);

private:
    uint8_t *data;
    // TODO: Implement mappers
};
