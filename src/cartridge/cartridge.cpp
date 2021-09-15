#include <fstream>
#include "cartridge/cartridge.h"

Cartridge::Cartridge(std::string path) {
    int file_size;
    std::ifstream cart(path, std::ios::binary);
    size = static_cast<int>(cart.tellg());
    cart.seekg(0, std::ios::end);
    size = static_cast<int>(cart.tellg()) - size;

    cart.seekg(0, std::ios::beg);
    data = new uint8_t[size];
}

Cartridge::~Cartridge() {
    delete[] data;
}

uint8_t Cartridge::read(uint16_t address) {
    return data[address];
}