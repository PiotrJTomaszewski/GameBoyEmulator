#include <iostream>
#include <fstream>
#include <cstring>
#include "bus.h"
#include "cartridge/cartridge_header.h"
#include "cartridge/rom_only_cart.h"
#include "emulator_exception.h"

// TODO: Allow accessing all types of memory "directly" using bus?

Bus::Bus() {
    is_cart_inserted = false;
    // TODO: Remove
    memset(tmp_mem, 0, 0xFFFF+1);
}

Bus::~Bus() {
    delete cartridge;
}

ReadWriteInterface *Bus::get_mem_access_handler(uint16_t address) {
    if (address <= 0x7FFF) { // Cartridge
        return cartridge;
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

void Bus::load_cartridge_from_file(std::string file_path) {
    int file_size;
    cardridge_header_t header;

    std::ifstream file(file_path, std::ios::binary);
    if (!file.good()) {
        throw std::runtime_error("Cannot open file: " + file_path);
    }
    file_size = static_cast<int>(file.tellg());
    file.seekg(0, std::ios::end);
    file_size = static_cast<int>(file.tellg()) - file_size;

    // TODO: Check if file is big enough to contain a header
    // Read cartridge header
    file.seekg(CARTRIDGE_HEADER_START, std::ios::beg);
    file.read(reinterpret_cast<char *>(&header), CARTRIDGE_HEADER_SIZE);
    switch(header.type) {
        case ROM_ONLY:
            cartridge = new ROMOnlyCart();
            break;
        default:
            throw EmulatorException("Cartridge type %s not supported yet", get_cartridge_type_name(header.type).c_str());
            break;
    }
    cartridge->load_from_file(file, file_size);

    // data = new uint8_t[file_size];
    // cart_file.read((char *)(data), file_size);
    file.close();
}

// void Bus::insert_cartridge(Cartridge* cartridge) {
//     this->cartridge = std::unique_ptr<Cartridge>(cartridge);
//     is_cart_inserted = true;
// }

// void Bus::remove_cartridge() {
//     cartridge.reset();
//     is_cart_inserted = false;
// }

bool Bus::get_is_cart_inserted() {
    return is_cart_inserted;
}

// void Bus::tmp_dump() {
//     std::fstream file;
//     file.open("mem.bin", std::ios::out|std::ios::binary);
//     file.write((char *)tmp_mem, 0xFFFF+1);
// }

// void Bus::tmp_load() {
//     std::fstream file;
//     file.open("mem.bin", std::ios::in|std::ios::binary);
//     file.read((char *)tmp_mem, 0xFFFF+1);
// }
