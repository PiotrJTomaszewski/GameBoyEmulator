#pragma once
#include <cstdint>

class VideoRAM {
public:
    VideoRAM();
    ~VideoRAM();
    static const int VRAM_SIZE = 8 * 1024;
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
private:
    uint8_t data[VRAM_SIZE];
};
