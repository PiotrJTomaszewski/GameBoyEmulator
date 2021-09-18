#pragma once
#include <cstdint>
#include <memory>
#include "cartridge/cartridge.h"
#include "io/io.h"

class Bus {
    friend class GUI; // TODO: Remove?
    friend class PPU;
public:
    Bus(IO &io);
    ~Bus();
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
    void insert_cartridge(Cartridge* cartridge);
    void remove_cartridge();
    bool get_is_cart_inserted();
    void tmp_dump();
    void tmp_load();
private:
    std::unique_ptr<Cartridge> cartridge;
    IO &io;
    bool is_cart_inserted;
    uint8_t tmp_mem[0xFFFF+1];
    ReadWriteInterface *get_mem_access_handler(uint16_t address);
};
