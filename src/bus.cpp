#include "bus.h"
#include <iostream>
#include <fstream>
#include <cstring>

// TODO: Allow accessing all types of memory "directly" using bus?

Bus::Bus(IO &io): io{io} {
    is_cart_inserted = false;
    // TODO: Remove
    memset(tmp_mem, 0, 0xFFFF+1);
}

Bus::~Bus() {

}

ReadWriteInterface *Bus::get_mem_access_handler(uint16_t address) {
    if (address <= 0x7FFF) { // Cartridge
        return cartridge.get();
    } else if ((address >= 0xFF00 && address <= 0xFF7F) || address == 0xFFFF) { // IO Registers
        return &io;
    } else {
        return nullptr;
    }
}

void Bus::write(uint16_t address, uint8_t value) {
    // std::cout << "Write 0x" << std::hex << static_cast<int>(address) << " -> 0x" << static_cast<int>(value) << std::endl;
    ReadWriteInterface *handler = get_mem_access_handler(address);
    if (handler != nullptr) {
        handler->write(address, value);
    } else {
        tmp_mem[address] = value;
    }
    tmp_mem[address] = value;
}

uint8_t Bus::read(uint16_t address) {
    uint8_t value = 0;
    ReadWriteInterface *handler = get_mem_access_handler(address);
    if (handler != nullptr) {
        value = handler->read(address);
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
