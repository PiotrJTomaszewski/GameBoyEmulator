#pragma once
#include "cpu/regs.h"
#include "bus.h"

enum intr_change_t {
    NO_CHANGE,
    ENABLE,
    DISABLE
};

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
    void restart();
    int next_cycle();
private:
    uint8_t regA;
    reg_16bit_t _regBC, _regDE, _regHL, _regPC, _regSP;
    flags_reg_t flags_reg;
    Bus &bus;
    
    intr_change_t intr_state_change;  // Should the interrupts be enabled/disabled after an instruction is executed
    bool interrupts_enabled;
    bool interrupt_requested;
    bool is_halted;

    uint8_t add8bit_with_flags(uint8_t val1, uint8_t val2, uint8_t carry);
    uint16_t add16bit_with_flags(uint16_t val1, uint16_t val2);
    uint8_t sub8bit_with_flags(uint8_t val1, uint8_t val2, uint8_t borrow);
    uint8_t inc8bit_with_flags(uint8_t val);
    uint8_t dec8bit_with_flags(uint8_t val);
    uint8_t and8bit_with_flags(uint8_t val1, uint8_t val2);
    uint8_t or8bit_with_flags(uint8_t val1, uint8_t val2);
    uint8_t xor8bit_with_flags(uint8_t val1, uint8_t val2);
    uint8_t swap_nibbles_with_flags(uint8_t val);
    uint8_t rotate_left_with_flags(uint8_t val);
    uint8_t rotate_left_carry_with_flags(uint8_t val);
    uint8_t rotate_right_with_flags(uint8_t val);
    uint8_t rotate_right_carry_with_flags(uint8_t val);
    uint8_t shift_left_with_flags(uint8_t val);
    uint8_t shift_right_leave_msb_with_flags(uint8_t val);
    uint8_t shift_right_with_flags(uint8_t val);
    inline void test_bit_with_flags(int bit_no, uint8_t val);
    inline void stack_push(uint8_t value);
    inline uint8_t stack_pop();
    inline uint8_t get_next_prog_byte();
    uint16_t get_next_2_prog_bytes();
    inline int cond_return(bool condition);
    inline void cond_jump(bool condition);
    int cond_call(bool condition);
    inline void call_addr(uint16_t addr);
    int cpu_exec_op(uint8_t opcode);
};
