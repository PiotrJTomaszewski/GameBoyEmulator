#include "wrappers/cpu_wrapper.h"

CPUWrapper::CPUWrapper(Bus &bus): CPU(bus) {

}

CPUWrapper::~CPUWrapper() {

}

void CPUWrapper::exec_explicit_instr(instruction_t instr) {
    cpu_exec_op(instr);
}

void CPUWrapper::set_regA(uint8_t value) {
    regA = value;
}

void CPUWrapper::set_regB(uint8_t value) {
    _regBC.pair.higher = value;
}

void CPUWrapper::set_regC(uint8_t value) {
    _regBC.pair.lower = value;
}

void CPUWrapper::set_regD(uint8_t value) {
    _regDE.pair.higher = value;
}

void CPUWrapper::set_regE(uint8_t value) {
    _regDE.pair.lower = value;
}

void CPUWrapper::set_regH(uint8_t value) {
    _regHL.pair.higher = value;
}

void CPUWrapper::set_regL(uint8_t value) {
    _regHL.pair.lower = value;
}

void CPUWrapper::set_regBC(uint16_t value) {
    _regBC.value = value;
}

void CPUWrapper::set_regDE(uint16_t value) {
    _regDE.value = value;
}

void CPUWrapper::set_regHL(uint16_t value) {
    _regHL.value = value;
}

void CPUWrapper::set_regPC(uint16_t value) {
    _regPC.value = value;
}

void CPUWrapper::set_regSP(uint16_t value) {
    _regSP.value = value;
}

void CPUWrapper::set_flag_C() {
    flags_reg.flags.C = 1;
}

void CPUWrapper::set_flag_H() {
    flags_reg.flags.H = 1;
}

void CPUWrapper::set_flag_N() {
    flags_reg.flags.N = 1;
}

void CPUWrapper::set_flag_Z() {
    flags_reg.flags.Z = 1;
}

void CPUWrapper::clear_flag_C() {
    flags_reg.flags.C = 0;
}

void CPUWrapper::clear_flag_H() {
    flags_reg.flags.H = 0;
}

void CPUWrapper::clear_flag_N() {
    flags_reg.flags.N = 0;
}

void CPUWrapper::clear_flag_Z() {
    flags_reg.flags.Z = 0;
}

unsigned CPUWrapper::get_flag_C() {
    return flags_reg.flags.C;
}

unsigned CPUWrapper::get_flag_H() {
    return flags_reg.flags.H;
}

unsigned CPUWrapper::get_flag_N() {
    return flags_reg.flags.N;
}

unsigned CPUWrapper::get_flag_Z() {
    return flags_reg.flags.Z;
}
