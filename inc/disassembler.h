#pragma once
#include <cstdint>
#include "cpu/common.h"

class Disassembler {
public:
    static int disassemble_instr(instruction_t const& instruction, char *buffer);
};
