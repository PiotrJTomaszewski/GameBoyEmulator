#include "memory/video_ram.h"

VideoRAM::VideoRAM() {

}

VideoRAM::~VideoRAM() {

}

void VideoRAM::write(uint16_t address, uint8_t value) {
    // TODO: Implement access control
    data[address - VRAM_MEMORY_START_ADDR] = value;
}

uint8_t VideoRAM::read(uint16_t address) {
    // TODO: Implement access control
    return data[address - VRAM_MEMORY_START_ADDR];
}

uint8_t *VideoRAM::get_raw_data() {
    return data;
}
