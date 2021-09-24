#pragma once

#include <fstream>
#include <cstdint>
#include <string>
#include "read_write_interface.h"

class Cartridge: public virtual ReadWriteInterface {
public:
    virtual ~Cartridge() {};
    virtual void load_from_file(std::ifstream &cart_file, int file_size) = 0;
    virtual void write(uint16_t address, uint8_t value) = 0;
    virtual uint8_t read(uint16_t address) = 0;
};
