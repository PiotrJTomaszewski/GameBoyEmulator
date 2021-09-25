#pragma once
#include <cstdint>
#include <fstream>
#include "cartridge/cartridge.h"
#include "cartridge/cartridge_header.h"

class MBC1Cart: public virtual Cartridge {
public:
    MBC1Cart(cardridge_header_t &header);
    ~MBC1Cart();
    void load_from_file(std::ifstream &cart_file, unsigned file_size);
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);

private:
    enum banking_mode_t {
        ROM_BANKING_MODE = 0,
        RAM_BANKING_MODE = 1
    };

private:
    static const unsigned SINGLE_ROM_BANK_SIZE = 0x4000;
    static const unsigned SINGLE_RAM_BANK_SIZE = 0x2000;
    uint8_t *ROM_data;
    uint8_t *RAM_data;
    bool RAM_enabled;
    unsigned selected_ROM_bank_lower_bits;
    unsigned number_of_ROM_banks;
    unsigned number_of_RAM_banks;
    unsigned selected_RAM_bank_or_upper_ROM_bits;
    banking_mode_t selected_banking_mode;
};
