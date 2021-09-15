#include "bus.h"
#include <iostream>

Bus::Bus() {

}

Bus::~Bus() {

}

void Bus::write(uint16_t address, uint8_t value) {
    std::cout << "Write 0x" << std::hex << static_cast<int>(address) << " -> 0x" << static_cast<int>(value) << std::endl;
}

uint8_t Bus::read(uint16_t address) {
    uint8_t value = 0;
    std::cout << "Read 0x" << std::hex << static_cast<int>(address) << " -> 0x" << static_cast<int>(value) << std::endl;
    return value;
}