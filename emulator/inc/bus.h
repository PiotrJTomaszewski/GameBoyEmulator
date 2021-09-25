#pragma once
#include <cstdint>
#include <memory>
#include "cartridge/cartridge.h"
#include "io/io.h"
#include "memory/video_ram.h"
#include "read_write_interface.h"

class Bus: public ReadWriteInterface {
    friend class GUI; // TODO: Remove?
public:
    Bus();
    Bus(const Bus&) = delete;
    ~Bus();
    Bus& operator=(const Bus&) = delete;
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
    // void insert_cartridge(Cartridge* cartridge);
    void load_cartridge_from_file(std::string file_path);
    // void remove_cartridge();
    bool get_is_cart_inserted();
    // void tmp_dump();
    // void tmp_load();
    IO io;
    VideoRAM vram;

protected:
    Cartridge *cartridge;
    bool is_cart_inserted;
    uint8_t tmp_mem[0xFFFF+1];
    ReadWriteInterface *get_mem_access_handler(uint16_t address);
};
