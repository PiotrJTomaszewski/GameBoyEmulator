#include "memory/video_ram.h"

VideoRAM::VideoRAM() {

}

VideoRAM::~VideoRAM() {

}

void VideoRAM::write(uint16_t address, uint8_t value) {
    data[address] = value;
}

uint8_t VideoRAM::read(uint16_t address) {
    return data[address];
}
