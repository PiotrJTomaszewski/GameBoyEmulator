#pragma once
#include "cpu/regs.h"
#include "bus.h"

class CPU {
public:
    CPU(Bus &bus);
    ~CPU();
    uint8_t get_regA() {return regA;};
    uint8_t get_regB() {return _regBC.pair.higher;};
    uint8_t get_regC() {return _regBC.pair.lower;};
    uint8_t get_regD() {return _regDE.pair.higher;};
    uint8_t get_regE() {return _regDE.pair.lower;};
    uint8_t get_regH() {return _regHL.pair.higher;};
    uint8_t get_regL() {return _regHL.pair.lower;};
    uint16_t get_regBC() {return _regBC.value;};
    uint16_t get_regDE() {return _regDE.value;};
    uint16_t get_regHL() {return _regHL.value;};
    uint16_t get_regPC() {return _regPC.value;};
    uint16_t get_regSP() {return _regSP.value;};
    flags_reg_t get_flags_reg() {return flags_reg;}
private:
    uint8_t regA;
    reg_16bit_t _regBC, _regDE, _regHL, _regPC, _regSP;
    flags_reg_t flags_reg;
    Bus &bus;
};
