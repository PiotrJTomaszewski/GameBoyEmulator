#pragma once
#include <cstdint>
#include <memory>
#include "cartridge/cartridge.h"

class Bus {
public:
    Bus();
    ~Bus();
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
    void insert_cartridge(Cartridge* cartridge);
    void remove_cartridge();
    bool get_is_cart_inserted();
private:
    std::unique_ptr<Cartridge> cartridge;
    bool is_cart_inserted;
    uint8_t tmp_mem[0xFFFF+1];
};
