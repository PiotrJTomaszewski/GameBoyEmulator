#pragma once
#include <cstdint>
#include "cpu/cpu.h"
#include "bus.h"

// CPU but with an option to manipulate it's internals
class CPUWrapper: public CPU {
public:
    CPUWrapper(Bus &bus, Logger &logger);
    ~CPUWrapper();
    void exec_explicit_instr(instruction_t instr);
    void set_regA(uint8_t value);
    void set_regB(uint8_t value);
    void set_regC(uint8_t value);
    void set_regD(uint8_t value);
    void set_regE(uint8_t value);
    void set_regH(uint8_t value);
    void set_regL(uint8_t value);
    void set_regBC(uint16_t value);
    void set_regDE(uint16_t value);
    void set_regHL(uint16_t value);
    void set_regPC(uint16_t value);
    void set_regSP(uint16_t value);
    void set_flag_C();
    void set_flag_H();
    void set_flag_N();
    void set_flag_Z();
    void clear_flag_C();
    void clear_flag_H();
    void clear_flag_N();
    void clear_flag_Z();
    unsigned get_flag_C();
    unsigned get_flag_H();
    unsigned get_flag_N();
    unsigned get_flag_Z();
};
