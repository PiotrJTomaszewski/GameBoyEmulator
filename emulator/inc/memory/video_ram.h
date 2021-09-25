#pragma once
#include <cstdint>
#include "read_write_interface.h"

class VideoRAM: public virtual ReadWriteInterface {
public:
    VideoRAM();
    ~VideoRAM();
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);

    /**
     * Returns pointer to the data array to allow PPU access VRAM directly.
     * Just to make VRAM access more efficient
     */
    uint8_t *get_raw_data();

private:
    static const int VRAM_SIZE = 0x2000;
    static const unsigned VRAM_MEMORY_START_ADDR = 0x8000;
    uint8_t data[VRAM_SIZE];
};
