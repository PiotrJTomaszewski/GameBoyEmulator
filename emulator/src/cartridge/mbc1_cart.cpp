#include "cartridge/mbc1_cart.h"
#include "emulator_exception.h"

// TODO: Add support for multi-game compilation carts
MBC1Cart::MBC1Cart(cardridge_header_t &header) {
    RAM_enabled = false;
    selected_ROM_bank_lower_bits = 0x01;
    number_of_ROM_banks = (1 << (header.ROM_size_shift + 1));
    switch(header.RAM_size_id) {
        case 0x02:
            number_of_RAM_banks = 1;
            break;
        case 0x03:
            number_of_RAM_banks = 4;
            break;
        case 0x04:
            number_of_RAM_banks = 16;
            break;
        case 0x05:
            number_of_RAM_banks = 8;
            break;
        default:
            number_of_RAM_banks = 0;
            break;
    }
    ROM_data = new uint8_t[number_of_ROM_banks * SINGLE_ROM_BANK_SIZE];
    RAM_data = new uint8_t[number_of_RAM_banks * SINGLE_RAM_BANK_SIZE];
    selected_banking_mode = ROM_BANKING_MODE;
    selected_RAM_bank_or_upper_ROM_bits = 0;
}

MBC1Cart::~MBC1Cart() {
    delete[] ROM_data;
    delete[] RAM_data;
}

void MBC1Cart::load_from_file(std::ifstream &cart_file, unsigned file_size) {
    if (file_size > (number_of_ROM_banks * SINGLE_ROM_BANK_SIZE)) {
        throw EmulatorException("File has incorrect size for ROM-only cart. Expected %d, got %d",
            number_of_ROM_banks * SINGLE_ROM_BANK_SIZE, file_size);
    }
    cart_file.seekg(0, std::ios::beg);
    cart_file.read(reinterpret_cast<char *>(ROM_data), file_size);

}

void MBC1Cart::write(uint16_t address, uint8_t value) {
    if (address <= 0x1FFF) { // RAM Enable
        RAM_enabled = ((value & 0x0F) == 0x0A); // 0xA in the lower nibble enables RAM
    } else if (address <= 0x3FFF) { // ROM bank number
        /*  Up to 5 bits (may be less), the rest is discarded
            If the cartridge has e.g. 16 banks, only 4 bits are needed to address them
            so the fifth one would be also discarded
            The amount of banks is always a power of 2, so we always need (amount - 1) bits to address them.
        */
        selected_ROM_bank_lower_bits = (value & (number_of_ROM_banks - 1));
        if (selected_ROM_bank_lower_bits == 0) { // Bank 0x00 cannot be accessed this way (and so banks 0x20, 0x40 and 0x60)
            selected_ROM_bank_lower_bits += 1;
        }
    } else if (address <= 0x5FFF) { // RAM bank address or upper bits of ROM bank number
        selected_RAM_bank_or_upper_ROM_bits = (value & 0b11);
    } else if (address <= 0x7FFF) { // ROM/RAM mode select
        selected_banking_mode = static_cast<banking_mode_t>(value);
    } else if (RAM_enabled && address >= 0xA000 && address <= 0xBFFF) { // RAM bank 00-03
        uint16_t in_bank_address = address - 0xA000;
        unsigned selected_RAM_bank = (selected_banking_mode == RAM_BANKING_MODE ? selected_RAM_bank_or_upper_ROM_bits : 0);
        if (selected_RAM_bank < number_of_RAM_banks) { // Bank IDs start at 0
            RAM_data[(selected_RAM_bank * SINGLE_RAM_BANK_SIZE) + in_bank_address] = value;
        }
    }
}

uint8_t MBC1Cart::read(uint16_t address) {
    if (address <= 0x3FFF) { // ROM bank 0
        return ROM_data[address];
    } else if (address <= 0x7FFF) { // ROM bank 01-7F
        uint16_t in_bank_address = address - 0x4000;
        unsigned selected_ROM_bank = selected_ROM_bank_lower_bits;
        if (selected_banking_mode == ROM_BANKING_MODE) {
            selected_ROM_bank |= (selected_RAM_bank_or_upper_ROM_bits << 5);
        }
        if (selected_ROM_bank < number_of_ROM_banks) { // Bank IDs start at 0
            return ROM_data[(selected_ROM_bank * SINGLE_ROM_BANK_SIZE) + in_bank_address];
        }
    } else if (RAM_enabled && address >= 0xA000 && address <= 0xBFFF) { // RAM bank 00-03
        uint16_t in_bank_address = address - 0xA000;
        unsigned selected_RAM_bank = (selected_banking_mode == RAM_BANKING_MODE ? selected_RAM_bank_or_upper_ROM_bits : 0);
        if (selected_RAM_bank < number_of_RAM_banks) { // Bank IDs start at 0
            return RAM_data[(selected_RAM_bank * SINGLE_RAM_BANK_SIZE) + in_bank_address];
        }
    }
    return 0xFF; // TODO: What should be returned
}
