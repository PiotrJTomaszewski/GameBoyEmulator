#pragma once
#include "read_write_interface.h"
#include "io/interrupts.h"

class IO: public ReadWriteInterface {
friend class PPU; // TODO: Remove friends
friend class GUI;
public:
    IO();
    ~IO();
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
    Interrupts interrupts;
private:
    uint8_t data[0x80];
};
