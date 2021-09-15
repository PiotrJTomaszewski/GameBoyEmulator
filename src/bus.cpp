#include "bus.h"
#include <iostream>
#include <cstring>

Bus::Bus() {
    is_cart_inserted = false;
    memset(tmp_mem, 0, 0xFFFF+1);
}

Bus::~Bus() {

}

void Bus::write(uint16_t address, uint8_t value) {
    // std::cout << "Write 0x" << std::hex << static_cast<int>(address) << " -> 0x" << static_cast<int>(value) << std::endl;
    tmp_mem[address] = value;
}

uint8_t Bus::read(uint16_t address) {
    uint8_t value = 0;
    if (address >= 0x0000 && address <= 0x7FFF) {
        value = cartridge.get()->read(address);
    } else {
        value = tmp_mem[address];
    }
    // std::cout << "Read 0x" << std::hex << static_cast<int>(address) << " -> 0x" << static_cast<int>(value) << std::endl;
    return value;
}

void Bus::insert_cartridge(Cartridge* cartridge) {
    this->cartridge = std::unique_ptr<Cartridge>(cartridge);
    is_cart_inserted = true;
}

void Bus::remove_cartridge() {
    cartridge.reset();
    is_cart_inserted = false;
}

bool Bus::get_is_cart_inserted() {
    return is_cart_inserted;
}
