#pragma once
#include <cstdint>
#include <vector>
#include "read_write_interface.h"

class Disassembler {
public:
    Disassembler();
    ~Disassembler();
    void disassemble_code(ReadWriteInterface &src, uint16_t start_addr, uint16_t end_addr);
    int disassemble_instr(ReadWriteInterface &src, uint16_t mem_addr, char *buffer);
    struct disassembled_t {
        int address;
        char text[20];
    };
    std::vector<disassembled_t> &get_disassembled_code();

private:
    std::vector<disassembled_t> disassembled;
};
