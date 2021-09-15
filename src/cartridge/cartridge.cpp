#include <fstream>
#include "cartridge/cartridge.h"

Cartridge::Cartridge(std::string path) {
    int file_size;
    std::ifstream cart_file(path, std::ios::binary);
    size = static_cast<int>(cart_file.tellg());
    cart_file.seekg(0, std::ios::end);
    size = static_cast<int>(cart_file.tellg()) - size;

    cart_file.seekg(0, std::ios::beg);
    data = new uint8_t[size];
    cart_file.read((char *)(data), size);
    cart_file.close();
}

Cartridge::~Cartridge() {
    delete[] data;
}

void Cartridge::write(uint16_t address, uint8_t value) {
    
}

uint8_t Cartridge::read(uint16_t address) {
    return data[address];
}