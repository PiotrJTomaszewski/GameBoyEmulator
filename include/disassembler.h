#pragma once
#include <cstdint>
#include <vector>
#include "bus.h"

class Disassembler {
public:
    Disassembler(Bus &bus);
     ~Disassembler();
private:
    Bus &bus;
    int disassembly_instr(uint16_t mem_addr, char *buffer);
};
