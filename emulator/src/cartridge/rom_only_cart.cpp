#include "cartridge/rom_only_cart.h"
#include "emulator_exception.h"

ROMOnlyCart::ROMOnlyCart() {
    
}

ROMOnlyCart::~ROMOnlyCart() {

}

void ROMOnlyCart::load_from_file(std::ifstream &cart_file, unsigned file_size) {
    if (file_size > MEMORY_SIZE) {
        throw EmulatorException("File has incorrect size for ROM-only cart. Expected %d, got %d", MEMORY_SIZE, file_size);
    }
    cart_file.seekg(0, std::ios::beg);
    cart_file.read(reinterpret_cast<char *>(&data), file_size);
}

void ROMOnlyCart::write(uint16_t address, uint8_t value) {
    /* Writes to ROM should be obviously futile but ROM only cartridge
     can optionally cantain up to 8kB of RAM located between 0xA000 and 0xBFFF
     */
    if (address >= 0xA000 || address <= 0xBFFF) {
        data[address] = value;
    }
}

uint8_t ROMOnlyCart::read(uint16_t address) {
    return data[address];
}

uint8_t *ROMOnlyCart::get_raw_ROM_data() {
    return data;
}

unsigned ROMOnlyCart::get_raw_ROM_size() {
    return MEMORY_SIZE;
}
