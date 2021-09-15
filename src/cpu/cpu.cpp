#include "bus.h"
#include "cpu/cpu.h"

// Macros for easier work with 16 bit registers
#define regBC _regBC.value
#define regB  _regBC.pair.higher
#define regC  _regBC.pair.lower
#define regDE _regDE.value
#define regD  _regDE.pair.higher
#define regE  _regDE.pair.lower
#define regHL _regHL.value
#define regH  _regHL.pair.higher
#define regL  _regHL.pair.lower
#define regPC _regPC.value
#define regPC_higher _regPC.pair.higher
#define regPC_lower _regPC.pair.lower
#define regSP _regSP.value
#define regSP_higher _regSP.pair.higher
#define regSP_lower _regSP.pair.lower

CPU::CPU(Bus &bus): bus{bus} {

}

CPU::~CPU() {

}
