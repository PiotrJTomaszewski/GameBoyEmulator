#pragma once

#include <cstdint>
#include <string>

class Cartridge {
public:
    Cartridge(std::string path);
    ~Cartridge();
    uint8_t read(uint16_t address);

private:
    int size;
    uint8_t *data;
    // TODO: Implement mappers
};
