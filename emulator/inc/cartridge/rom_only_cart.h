#pragma once
#include <cstdint>
#include <fstream>
#include "cartridge/cartridge.h"
#include "cartridge/cartridge_header.h"

class ROMOnlyCart: public virtual Cartridge {
public:
    ROMOnlyCart();
    ~ROMOnlyCart();
    void load_from_file(std::ifstream &cart_file, unsigned file_size);
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);

private:
    static const unsigned MEMORY_SIZE = 0x8000;
    uint8_t data[MEMORY_SIZE];
};
