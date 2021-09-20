#include <cstring>
#include "bus.h"
#include "cpu/cpu.h"

/**
 * Converts two 8bit numbers to one 16bit number
 */
#define join_bytes(higher, lower) ((higher << 8) | lower)

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
    restart();
}

CPU::~CPU() {

}

/**
 * Sets CPU registers and helper variables to their initial values
 */
void CPU::restart() {
    regPC = 0x100;
    regSP = 0xFFFE;
    regA = 0x01;
    regBC = 0x0013;
    regDE = 0x00D8;
    regHL = 0x014D;
    flags_reg.value = 0x00;
    is_halted = false;
    is_stopped = false;
}

/**
 * Executes a signle machine cylce on the CPU
 * Returns the number of clock cycles this step took
 */
// TODO: Update
// TODO: While CPU executes an operation it fetches the next one - emulate that. Also, update the cycle count - currently it doesn't reflect that behaviour
int CPU::next_cycle() {
    int cycles = 0;
    bool old_intrs_should_be_enabled = bus.io.interrupts.get_intrs_should_be_enabled();
    intr_type_t ready_interrupt = bus.io.interrupts.get_ready_interrupt();
    if (ready_interrupt != NO_INTERRUPT) {
        cycles += 5; // Preparation for interrupt execution takes 5 cycles
        call_addr(INTERRUPT_PC_LOOKUP[ready_interrupt]);
        bus.io.interrupts.all_interrupts_disable();
        bus.io.interrupts.mark_used(ready_interrupt);
        is_halted = false;
    }
    if (!(is_halted || is_stopped)) {
        cycles += cpu_exec_op(get_next_prog_byte());
    } else {
        /* The processor is usually emulated in batches
        * so to avoid being stuck in an infinite loop
        * I've assumed that halted processor executes NOPs 
        */
        cycles += 4;
    }
    bus.io.interrupts.intrs_update_state(old_intrs_should_be_enabled);
    return cycles;
}

long CPU::get_clock_speed_Hz() {
    return CLOCK_SPEED_HZ;
}

inline int8_t unsigned_byte_to_signed(uint8_t ubyte) {
    int8_t sbyte;
    memcpy(&sbyte, &ubyte, 1);
    return sbyte;
}

/**
 * Adds two 8bit values with carry and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::add8bit_with_flags(uint8_t val1, uint8_t val2, uint8_t carry) {
    int result = val1 + val2 + carry;
    uint8_t result8bit = result & 0xFF;
    flags_reg.flags.Z = (result8bit == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = (((val1 & 0x0F) + (val2 & 0x0F) + carry) > 0x0F);
    flags_reg.flags.C = (result >= 0x100);
    return result8bit;
}

/**
 * Adds two 16bit values and sets the carry flag acoordingly
 * Affected flags: N, H, C
 * Affected registers: None
 */
uint16_t CPU::add16bit_with_flags(uint16_t val1, uint16_t val2) {
    int result = val1 + val2;
    flags_reg.flags.N = 0;
    flags_reg.flags.H = (((val1 & 0xFFF) + (val2 & 0xFFF)) > 0xFFF); // TODO: Check if it's calculated properly
    flags_reg.flags.C = (result >= 0x10000);
    return result & 0xFFFF;
}

/**
 * Adds a signed 8 bit value to a 16bit unsigned one and sets the carry flag acoordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint16_t CPU::add_s8bit_to_u16bit_with_flags(int8_t val1, uint16_t val2) {
    int result = val1 + val2;
    uint16_t result16bit = result & 0xFFFF;
    flags_reg.flags.Z = (result16bit == 0);
    flags_reg.flags.N = 0;
    // Flags behave as if we were adding 8 bit values
    flags_reg.flags.H = (((val1 & 0x0F) + (val2 & 0x0F)) > 0x0F);
    flags_reg.flags.C = (result >= 0x100);
    return result16bit;
}

/**
 * Substracts two 8bit values with borrow and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::sub8bit_with_flags(uint8_t val1, uint8_t val2, uint8_t borrow) {
    int result = val1 - val2 - borrow;
    uint8_t result8bit = result & 0xFF;
    flags_reg.flags.Z = (result8bit == 0);
    flags_reg.flags.N = 1;
    // TODO: Check if H and C flags are set correctly
    flags_reg.flags.H = (((val1 & 0x0F) + (~val2 & 0x0F) + (borrow^1)) > 0x0F);
    flags_reg.flags.C = ((val2+borrow) > val1); 
    return result8bit;
}

/**
 * Increments an 8bit value by 1 and sets the flags accordingly
 * Affected flags: Z, N, H
 * Affected registers: None
 */
uint8_t CPU::inc8bit_with_flags(uint8_t val) {
    uint8_t result = (val + 1) & 0xFF;
    flags_reg.flags.Z = (result == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = (((val & 0x0F) + 1) > 0x0F);
    return result;
}

/**
 * Decrements an 8bit value by 1 and sets the flags accordingly
 * Affected flags: Z, N, H
 * Affected registers: None
 */
uint8_t CPU::dec8bit_with_flags(uint8_t val) {
    uint8_t result = (val - 1) & 0xFF;
    flags_reg.flags.Z = (result == 0);
    flags_reg.flags.N = 1;
    // TODO: Check if H flag calculation is correct
    /* The whole calculation here should look like the one in 'sub8bit_with_flags'
     * but because 'val2' is always equal 1 and 'borrow' is always equal 0
     * we can write ((val1 & 0x0F) + (~1 & 0x0F) + (0^1)) > 0x0F
     * and the two latter values are equal: 0x0E + 1 = 0x0F.
     * So in the end it can be simplified to (val & 0x0F) + 0x0F > 0x0F
    */
    flags_reg.flags.H = ((val & 0x0F) > 0);
    return result;
}

/**
 * Calculates a logical AND of two 8bit values and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::and8bit_with_flags(uint8_t val1, uint8_t val2) {
    uint8_t result = val1 & val2;
    flags_reg.flags.Z = (result == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 1;
    flags_reg.flags.C = 0;
    return result;
}

/**
 * Calculates a logical OR of two 8bit values and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::or8bit_with_flags(uint8_t val1, uint8_t val2) {
    uint8_t result = val1 | val2;
    flags_reg.flags.Z = (result == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    flags_reg.flags.C = 0;
    return result;
}

/**
 * Calculates a logical XOR of two 8bit values and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::xor8bit_with_flags(uint8_t val1, uint8_t val2) {
    uint8_t result = val1 ^ val2;
    flags_reg.flags.Z = (result == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    flags_reg.flags.C = 0;
    return result;
}

/**
 * Swaps upper and lower nibbles and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::swap_nibbles_with_flags(uint8_t val) {
    // Result of the swap operation will be equal 0 only if it was equal before
    flags_reg.flags.Z = (val == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    flags_reg.flags.C = 0;
    return (val >> 4) | ((val & 0x0F) << 4);
}

/**
 * Rotates the number left and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::rotate_left_with_flags(uint8_t val) {
    flags_reg.flags.C = ((val & 0x80) != 0);
    val = (val << 1) | flags_reg.flags.C;
    flags_reg.flags.Z = (val == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    return val;
}

/**
 * Rotates the number left through Carry flag and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::rotate_left_carry_with_flags(uint8_t val) {
    uint8_t old_C_flag = flags_reg.flags.C;
    flags_reg.flags.C = ((val & 0x80) != 0);
    val = (val << 1) | old_C_flag;
    flags_reg.flags.Z = (val == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    return val;
}

/**
 * Rotates the number right and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::rotate_right_with_flags(uint8_t val) {
    flags_reg.flags.C = (val & 0x01);
    val = (val >> 1) | (flags_reg.flags.C << 7);
    flags_reg.flags.Z = (val == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    return val;
}

/**
 * Rotates the number right through Carry flag and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::rotate_right_carry_with_flags(uint8_t val) {
    uint8_t old_C_flag = flags_reg.flags.C;
    flags_reg.flags.C = (val & 0x01);
    val = (val >> 1) | (old_C_flag << 7);
    flags_reg.flags.Z = (val == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    return val;
}

/**
 * Shifts the number left and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::shift_left_with_flags(uint8_t val) {
    flags_reg.flags.C = ((val & 0x80) != 0);
    val = val << 1;
    flags_reg.flags.Z = (val == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    return val;
}

/**
 * Shifts the number right without changing MSB and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::shift_right_leave_msb_with_flags(uint8_t val) {
    uint8_t old_msb = val & 0x80;
    flags_reg.flags.C = (val & 0x01);
    val = (val >> 1) | old_msb;
    flags_reg.flags.Z = (val == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    return val;
}

/**
 * Shifts the number right and sets the flags accordingly
 * Affected flags: Z, N, H, C
 * Affected registers: None
 */
uint8_t CPU::shift_right_with_flags(uint8_t val) {
    flags_reg.flags.C = (val & 0x01);
    val = (val >> 1);
    flags_reg.flags.Z = (val == 0);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 0;
    return val;
}

/**
 * Reads specified bit of the value and sets the flags accordingly
 * Affected flags: Z, N, H
 * Affected registers: None
 */
inline void CPU::test_bit_with_flags(int bit_no, uint8_t val) {
    flags_reg.flags.Z = ~(val >> bit_no);
    flags_reg.flags.N = 0;
    flags_reg.flags.H = 1;
}

/**
 * Pushes an 8bit value on the stack
 * Affected flags: None
 * Affected registers: SP
 */
inline void CPU::stack_push(uint8_t value) {
    bus.write(--regSP, value);
}

/**
 * Pops an 8bit value from the stack
 * Affected flags: None
 * Affected registers: SP
 */
inline uint8_t CPU::stack_pop() {
    return bus.read(regSP++);
}

/**
 * Returns the next program byte from memory
 * Affected flags: None
 * Affected registers: PC
 */
inline uint8_t CPU::get_next_prog_byte() {
    return bus.read(regPC++);
}

/**
 * Returns the next two program bytes from memory as an 16bit value
 * Affected flags: None
 * Affected registers: PC
 */
uint16_t CPU::get_next_2_prog_bytes() {
    uint8_t lower = bus.read(regPC++);
    uint8_t higher = bus.read(regPC++);
    return join_bytes(higher, lower);
}

/**
 * Sets the new PC value (subroutine return) if the condition is met
 * Returns the number of clock cycles this operation takes
 * Affected flags: None
 * Affected registers: PC, SP
 */
// TODO: Update
inline int CPU::cond_return(bool condition) {
    if (condition) {
        regPC_lower = stack_pop();
        regPC_higher = stack_pop();
        return 11;
    } else {
        return 5;
    }
}

/**
 * Sets the new PC value (jump) if the condition is met
 * Returns the number of clock cycles this operation takes
 * Affected flags: None
 * Affected registers: PC
 */
inline void CPU::cond_jump(bool condition) {
    if (condition) {
        regPC = get_next_2_prog_bytes();
    } else {
        regPC += 2;
    }
}

/**
 * Sets the new PC value (subroutine call) if the condition is met
 * Returns the number of clock cycles this operation takes
 * Affected flags: None
 * Affected registers: PC, SP
 */
// TODO: Update
int CPU::cond_call(bool condition) {
    if (condition) {
        uint16_t newPC = get_next_2_prog_bytes();
        stack_push(regPC_higher);
        stack_push(regPC_lower);
        regPC = newPC;
        return 17;
    } else {
        regPC += 2;
        return 11;
    }
}

/**
 * Sets the new PC value and pushes PC on the stack (subroutine call)
 * Affected flags: None
 * Affected registers: PC, SP
 */
inline void CPU::call_addr(uint16_t addr) {
    stack_push(regPC_higher);
    stack_push(regPC_lower);
    regPC = addr;
}

inline void CPU::stop() {
    is_stopped = true;
    // TODO: Stop the LCD
    bus.io.timer.stop_DIV();
}

inline void CPU::run_after_stop() {
    is_stopped = false;
    // TODO: Run the LCD
    bus.io.timer.run_DIV_after_stop();
}

/**
 * Executes an operation specified by a given opcode on the CPU
 * Returns the number of clock cycles this operation takes
 */
int CPU::cpu_exec_op(uint8_t opcode) {
    int operation_cycles = -1;
    switch (opcode) {
        case 0x00: // NOP; 1 byte; 4 cycles
            operation_cycles = 4;
            break;
        case 0x01: // LD BC,nn; 3 bytes; 12 cycles
            regC = get_next_prog_byte();
            regB = get_next_prog_byte();
            operation_cycles = 12;
            break;
        case 0x02: // LD (BC),A; 1 byte; 8 cycles
            bus.write(regBC, regA);
            operation_cycles = 8;
            break;
        case 0x03: // INC BC; 1 byte; 8 cycles
            regBC = (regBC + 1) & 0xFFFF;
            operation_cycles = 8;
            break;
        case 0x04: // INC B; 1 byte; 4 cycles; Z,N,H flags
            regB = inc8bit_with_flags(regB);
            operation_cycles = 4;
            break;
        case 0x05: // DEC B; 1 byte; 4 cycles; Z,N,H flags
            regB = dec8bit_with_flags(regB);
            operation_cycles = 4;
            break;
        case 0x06: // LD B,n; 2 bytes; 8 cycles
            regB = get_next_prog_byte();
            operation_cycles = 8;
            break;
        case 0x07: // RLCA; 1 byte; 4 cycles; Z,N,H,C flags
            regA = rotate_left_with_flags(regA);
            operation_cycles = 4;
            break;
        case 0x08: // LD (nn),SP; 3 bytes; 20 cycles
            bus.write(get_next_2_prog_bytes(), regSP);
            operation_cycles = 20;
            break;
        case 0x09: // ADD HL,BC; 1 byte; 8 cycles; N,H,C flags
            regHL = add16bit_with_flags(regHL, regBC);
            operation_cycles = 8;
            break;
        case 0x0A: // LD A,(BC); 1 byte; 8 cycles
            regA = bus.read(regBC);
            operation_cycles = 8;
            break;
        case 0x0B: // DEC BC; 1 byte; 8 cycles
            regBC = (regBC - 1) & 0xFFFF;
            operation_cycles = 8;
            break;
        case 0x0C: // INC C; 1 byte; 4 cycles; Z,N,H flags
            regC = inc8bit_with_flags(regC);
            operation_cycles = 4;
            break;
        case 0x0D: // DEC C; 1 byte; 4 cycles; Z,N,H flags
            regC = dec8bit_with_flags(regC);
            operation_cycles = 4;
            break;
        case 0x0E: // LD C,n; 2 bytes; 8 cycles
            regC = get_next_prog_byte();
            operation_cycles = 8;
            break;
        case 0x0F: // RRCA; 1 byte; 4 cycles; Z,N,H,C flags
            regA = rotate_right_with_flags(regA);
            operation_cycles = 4;
            break;
        case 0x10: // Extended instructions // TODO: It's not really extended since it has only 1 possible operation
            switch (get_next_prog_byte()) {
            case 0x00: // STOP; 2 bytes; 4 cycles
                // TODO: Halt the LCD
                stop();
                operation_cycles = 4;
                break;
            default:
                break;
            }
            break;
        case 0x11: // LD DE,nn; 3 bytes; 12 cycles
            regE = get_next_prog_byte();
            regD = get_next_prog_byte();
            operation_cycles = 12;
            break;
        case 0x12: // LD (DE),A; 1 byte; 8 cycles
            bus.write(regDE, regA);
            operation_cycles = 8;
            break;
        case 0x13: // INC DE; 1 byte; 8 cycles
            regDE = (regDE + 1) & 0xFFFF;
            operation_cycles = 8;
            break;
        case 0x14: // INC D; 1 byte; 4 cycles; Z,N,H flags
            regD = inc8bit_with_flags(regD);
            operation_cycles = 4;
            break;
        case 0x15: // DEC D; 1 byte; 4 cycles; Z,N,H flags
            regD = dec8bit_with_flags(regD);
            operation_cycles = 4;
            break;
        case 0x16: // LD D,n; 2 bytes; 8 cycles
            regD = get_next_prog_byte();
            operation_cycles = 8;
            break;
        case 0x17: // RLA; 1 byte; 4 cycles; Z,N,H,C flags
            regA = rotate_left_carry_with_flags(regA);
            operation_cycles = 4;
            break;
        case 0x18: // JR n; 2 bytes; 8 cycles
            {
                int8_t offset = unsigned_byte_to_signed(get_next_prog_byte());
                regPC = (regPC + offset) & 0xFFFF;
            }
            operation_cycles = 8;
            break;
        case 0x19: // ADD HL,DE; 1 byte; 8 cycles; N,H,C flags
            regHL = add16bit_with_flags(regHL, regDE);
            operation_cycles = 8;
            break;
        case 0x1A: // LD A,(DE); 1 byte; 8 cycles
            regA = bus.read(regDE);
            operation_cycles = 8;
            break;
        case 0x1B: // DEC DE; 1 byte; 8 cycles
            regDE = (regDE - 1) & 0xFFFF;
            operation_cycles = 8;
            break;
        case 0x1C: // INC E; 1 byte; 4 cycles; Z,N,H flags
            regE = inc8bit_with_flags(regE);
            operation_cycles = 4;
            break;
        case 0x1D: // DEC E; 1 byte; 4 cycles; Z,N,H flags
            regE = dec8bit_with_flags(regE);
            operation_cycles = 4;
            break;
        case 0x1E: // LD E,n; 2 bytes; 8 cycles
            regE = get_next_prog_byte();
            operation_cycles = 8;
            break;
        case 0x1F: // RRA; 1 byte; 4 cycles; Z,N,H,C flags
            regA = rotate_right_carry_with_flags(regA);
            operation_cycles = 4;
            break;
        case 0x20: // JR NZ,n; 2 bytes; 8 cycles
            {
                int8_t signed_offset = unsigned_byte_to_signed(get_next_prog_byte());
                if (flags_reg.flags.Z == 0) {
                    regPC = (regPC + signed_offset) & 0xFFFF;
                }
            }
            operation_cycles = 8;
            break;
        case 0x21: // LD HL,nn; 3 bytes; 12 cycles
            regL = get_next_prog_byte();
            regH = get_next_prog_byte();
            operation_cycles = 12;
            break;
        case 0x22: // LD (HL+),A; 1 byte; 8 cycles
            bus.write(regHL++, regA);
            operation_cycles = 8;
            break;
        case 0x23: // INC HL; 1 byte; 8 cycles
            regHL = (regHL + 1) & 0xFFFF;
            operation_cycles = 8;
            break;
        case 0x24: // INC H; 1 byte; 4 cycles; Z,N,H flags
            regH = inc8bit_with_flags(regH);
            operation_cycles = 4;
            break;
        case 0x25: // DEC H; 1 byte; 4 cycles; Z,N,H flags
            regH = dec8bit_with_flags(regH);
            operation_cycles = 4;
            break;
        case 0x26: // LD H,n; 2 bytes; 8 cycles
            regH = get_next_prog_byte();
            operation_cycles = 8;
            break;
        case 0x27: // DAA; 1 byte; 4 cycles; Z,H,C flags
            if (flags_reg.flags.N == 0) {
                if (flags_reg.flags.C != 0 || (regA > 0x99)) {
                    regA = (regA + 0x60) & 0xFF;
                    flags_reg.flags.C = 1;
                }
                if (flags_reg.flags.H != 0 || ((regA & 0x0F) > 0x09)) {
                    regA = (regA + 0x06) & 0xFF;
                }
            } else {
                if (flags_reg.flags.C != 0) {
                    regA = (regA - 0x60) & 0xFF;
                }
                if (flags_reg.flags.H != 0) {
                    regA = (regA - 0x06) & 0xFF;
                }
            }

            flags_reg.flags.Z = (regA == 0) ? 1 : 0;
            flags_reg.flags.H = 0;
            operation_cycles = 4;
            break;
        case 0x28: // JR Z,n; 2 bytes; 8 cycles
            {
                int8_t offset = unsigned_byte_to_signed(get_next_prog_byte());
                if (flags_reg.flags.Z == 1) {
                    regPC = (regPC + offset) & 0xFFFF;
                }
            }
            operation_cycles = 8;
            break;
        case 0x29: // ADD HL,HL; 1 byte; 8 cycles; N,H,C flags
            regHL = add16bit_with_flags(regHL, regHL);
            operation_cycles = 8;
            break;
        case 0x2A: // LD A,(HL+); 1 byte; 8 cycles
            regA = bus.read(regHL++);
            operation_cycles = 8;
            break;
        case 0x2B: // DEC HL; 1 byte; 8 cycles
            regHL = (regHL - 1) & 0xFFFF;
            operation_cycles = 8;
            break;
        case 0x2C: // INC L; 1 byte; 4 cycles; Z,N,H flags
            regL = inc8bit_with_flags(regL);
            operation_cycles = 4;
            break;
        case 0x2D: // DEC L; 1 byte; 4 cycles; Z,N,H flags
            regL = dec8bit_with_flags(regL);
            operation_cycles = 4;
            break;
        case 0x2E: // LD L,n; 2 bytes; 8 cycles
            regL = get_next_prog_byte();
            operation_cycles = 8;
            break;
        case 0x2F: // CPL; 1 byte; 4 cycles; N,H flags
            regA = ~regA;
            flags_reg.flags.N = 1;
            flags_reg.flags.H = 1;
            operation_cycles = 4;
            break;
        case 0x30: // JR NC,n; 2 bytes; 8 cycles
            {
                int8_t offset = unsigned_byte_to_signed(get_next_prog_byte());
                if (flags_reg.flags.C == 0) {
                    regPC = (regPC + offset) & 0xFFFF;
                }
            }
            operation_cycles = 8;
            break;
        case 0x31: // LD SP,nn; 3 bytes; 12 cycles
            regSP_lower = get_next_prog_byte();
            regSP_higher = get_next_prog_byte();
            operation_cycles = 12;
            break;
        case 0x32: // LD (HL-),A; 1 byte; 8 cycles
            bus.write(regHL--, regA);
            operation_cycles = 8;
            break;
        case 0x33: // INC SP; 1 byte; 8 cycles
            regSP = (regSP + 1) & 0xFFFF;
            operation_cycles = 8;
            break;
        case 0x34: // INC (HL); 1 byte; 12 cycles; Z,N,H flags
            bus.write(regHL, inc8bit_with_flags(bus.read(regHL)));
            operation_cycles = 12;
            break;
        case 0x35: // DEC (HL); 1 byte; 12 cycles; Z,N,H flags
            bus.write(regHL, dec8bit_with_flags(bus.read(regHL)));
            operation_cycles = 12;
            break;
        case 0x36: // LD (HL),n; 2 bytes; 12 cycles
            bus.write(regHL, get_next_prog_byte());
            operation_cycles = 12;
            break;
        case 0x37: // STC; 1 byte; 4 cycles; N,H,C flag
            flags_reg.flags.N = 0;
            flags_reg.flags.H = 0;
            flags_reg.flags.C = 1;
            operation_cycles = 4;
            break;
        case 0x38: // JR C,n; 2 bytes; 8 cycles
            {
                int8_t offset = unsigned_byte_to_signed(get_next_prog_byte());
                if (flags_reg.flags.C == 1) {
                    regPC = (regPC + offset) & 0xFFFF;
                }
            }
            operation_cycles = 8;
            break;
        case 0x39: // ADD HL,SP; 1 byte; 8 cycles; N,H,C flags
            regHL = add16bit_with_flags(regHL, regSP);
            operation_cycles = 8;
            break;
        case 0x3A: // LD A,(HL-); 1 byte; 8 cycles
            regA = bus.read(regHL--);
            operation_cycles = 8;
            break;
        case 0x3B: // DEC SP; 1 byte; 8 cycles
            regSP = (regSP - 1) & 0xFFFF;
            operation_cycles = 8;
            break;
        case 0x3C: // INC A; 1 byte; 4 cycles; Z,N,H flags
            regA = inc8bit_with_flags(regA);
            operation_cycles = 4;
            break;
        case 0x3D: // DEC A; 1 byte; 4 cycles; Z,N,H flags
            regA = dec8bit_with_flags(regA);
            operation_cycles = 4;
            break;
        case 0x3E: // LD A,n; 2 bytes; 8 cycles
            regA = get_next_prog_byte();
            operation_cycles = 8;
            break;
        case 0x3F: // CCF; 1 byte; 4 cycles; N, H, C flags
            flags_reg.flags.N = 0;
            flags_reg.flags.H = 0;
            flags_reg.flags.C = ~flags_reg.flags.C;
            operation_cycles = 4;
            break;
        case 0x40: // LD B,B; 1 byte; 4 cycles
            operation_cycles = 4;
            break;
        case 0x41: // LD B,C; 1 byte; 4 cycles
            regB = regC;
            operation_cycles = 4;
            break;
        case 0x42: // LD B,D; 1 byte; 4 cycles
            regB = regD;
            operation_cycles = 4;
            break;
        case 0x43: // LD B,E; 1 byte; 4 cycles
            regB = regE;
            operation_cycles = 4;
            break;
        case 0x44: // LD B,H; 1 byte; 4 cycles
            regB = regH;
            operation_cycles = 4;
            break;
        case 0x45: // LD B,L; 1 byte; 4 cycles
            regB = regL;
            operation_cycles = 4;
            break;
        case 0x46: // LD B,(HL); 1 byte; 8 cycles
            regB = bus.read(regHL);
            operation_cycles = 8;
            break;
        case 0x47: // LD B,A; 1 byte; 4 cycles
            regB = regA;
            operation_cycles = 4;
            break;
        case 0x48: // LD C,B; 1 byte; 4 cycles
            regC = regB;
            operation_cycles = 4;
            break;
        case 0x49: // LD C,C; 1 byte; 4 cycles
            operation_cycles = 4;
            break;
        case 0x4A: // LD C,D; 1 byte; 4 cycles
            regC = regD;
            operation_cycles = 4;
            break;
        case 0x4B: // LD C,E; 1 byte; 4 cycles
            regC = regE;
            operation_cycles = 4;
            break;
        case 0x4C: // LD C,H; 1 byte; 4 cycles
            regC = regH;
            operation_cycles = 4;
            break;
        case 0x4D: // LD C,L; 1 byte; 4 cycles
            regC = regL;
            operation_cycles = 4;
            break;
        case 0x4E: // LD C,(HL); 1 byte; 8 cycles
            regC = bus.read(regHL);
            operation_cycles = 8;
            break;
        case 0x4F: // LD C,A; 1 byte; 4 cycles
            regC = regA;
            operation_cycles = 4;
            break;
        case 0x50: // LD D,B; 1 byte; 4 cycles
            regD = regB;
            operation_cycles = 4;
            break;
        case 0x51: // LD D,C; 1 byte; 4 cycles
            regD = regC;
            operation_cycles = 4;
            break;
        case 0x52: // LD D,D; 1 byte; 4 cycles
            operation_cycles = 4;
            break;
        case 0x53: // LD D,E; 1 byte; 4 cycles
            regD = regE;
            operation_cycles = 4;
            break;
        case 0x54: // LD D,H; 1 byte; 4 cycles
            regD = regH;
            operation_cycles = 4;
            break;
        case 0x55: // LD D,L; 1 byte; 4 cycles
            regD = regL;
            operation_cycles = 4;
            break;
        case 0x56: // LD D,(HL); 1 byte; 8 cycles
            regD = bus.read(regHL);
            operation_cycles = 8;
            break;
        case 0x57: // LD D,A; 1 byte; 4 cycles
            regD = regA;
            operation_cycles = 4;
            break;
        case 0x58: // LD E,B; 1 byte; 4 cycles
            regE = regB;
            operation_cycles = 4;
            break;
        case 0x59: // LD E,C; 1 byte; 4 cycles
            regE = regC;
            operation_cycles = 4;
            break;
        case 0x5A: // LD E,D; 1 byte; 4 cycles
            regE = regD;
            operation_cycles = 4;
            break;
        case 0x5B: // LD E,E; 1 byte; 4 cycles
            operation_cycles = 4;
            break;
        case 0x5C: // LD E,H; 1 byte; 4 cycles
            regE = regH;
            operation_cycles = 4;
            break;
        case 0x5D: // LD E,L; 1 byte; 4 cycles
            regE = regL;
            operation_cycles = 4;
            break;
        case 0x5E: // LD E,(HL); 1 byte; 8 cycles
            regE = bus.read(regHL);
            operation_cycles = 8;
            break;
        case 0x5F: // LD E,A; 1 byte; 4 cycles
            regE = regA;
            operation_cycles = 4;
            break;
        case 0x60: // LD H,B; 1 byte; 4 cycles
            regH = regB;
            operation_cycles = 4;
            break;
        case 0x61: // LD H,C; 1 byte; 4 cycles
            regH = regC;
            operation_cycles = 4;
            break;
        case 0x62: // LD H,D; 1 byte; 4 cycles
            regH = regD;
            operation_cycles = 4;
            break;
        case 0x63: // LD H,E; 1 byte; 4 cycles
            regH = regE;
            operation_cycles = 4;
            break;
        case 0x64: // LD H,H; 1 byte; 4 cycles
            operation_cycles = 4;
            break;
        case 0x65: // LD H,L; 1 byte; 4 cycles
            regH = regL;
            operation_cycles = 4;
            break;
        case 0x66: // LD H,(HL); 1 byte; 8 cycles
            regH = bus.read(regHL);
            operation_cycles = 8;
            break;
        case 0x67: // LD H,A; 1 byte; 4 cycles
            regH = regA;
            operation_cycles = 4;
            break;
        case 0x68: // LD L,B; 1 byte; 4 cycles
            regL = regB;
            operation_cycles = 4;
            break;
        case 0x69: // LD L,C; 1 byte; 4 cycles
            regL = regC;
            operation_cycles = 4;
            break;
        case 0x6A: // LD L,D; 1 byte; 4 cycles
            regL = regD;
            operation_cycles = 4;
            break;
        case 0x6B: // LD L,E; 1 byte; 4 cycles
            regL = regE;
            operation_cycles = 4;
            break;
        case 0x6C: // LD L,H; 1 byte; 4 cycles
            regL = regH;
            operation_cycles = 4;
            break;
        case 0x6D: // LD L,L; 1 byte; 4 cycles
            operation_cycles = 4;
            break;
        case 0x6E: // LD L,(HL); 1 byte; 8 cycles
            regL = bus.read(regHL);
            operation_cycles = 8;
            break;
        case 0x6F: // LD L,A; 1 byte; 4 cycles
            regL = regA;
            operation_cycles = 4;
            break;
        case 0x70: // LD (HL),B; 1 byte; 8 cycles
            bus.write(regHL, regB);
            operation_cycles = 8;
            break;
        case 0x71: // LD (HL),C; 1 byte; 8 cycles
            bus.write(regHL, regC);
            operation_cycles = 8;
            break;
        case 0x72: // LD (HL),D; 1 byte; 8 cycles
            bus.write(regHL, regD);
            operation_cycles = 8;
            break;
        case 0x73: // LD (HL),E; 1 byte; 8 cycles
            bus.write(regHL, regE);
            operation_cycles = 8;
            break;
        case 0x74: // LD (HL),H; 1 byte; 8 cycles
            bus.write(regHL, regH);
            operation_cycles = 8;
            break;
        case 0x75: // LD (HL),L; 1 byte; 8 cycles
            bus.write(regHL, regL);
            operation_cycles = 8;
            break;
        case 0x76: // HALT; 1 byte; 4 cycles
            is_halted = true;
            // TODO: If interrupts are disabled the next instruction should be skipped
            operation_cycles = 4;
            break;
        case 0x77: // LD (HL),A; 1 byte; 8 cycles
            bus.write(regHL, regA);
            operation_cycles = 8;
            break;
        case 0x78: // LD A,B; 1 byte; 4 cycles
            regA = regB;
            operation_cycles = 4;
            break;
        case 0x79: // LD A,C; 1 byte; 4 cycles
            regA = regC;
            operation_cycles = 4;
            break;
        case 0x7A: // LD A,D; 1 byte; 4 cycles
            regA = regD;
            operation_cycles = 4;
            break;
        case 0x7B: // LD A,E; 1 byte; 4 cycles
            regA = regE;
            operation_cycles = 4;
            break;
        case 0x7C: // LD A,H; 1 byte; 4 cycles
            regA = regH;
            operation_cycles = 4;
            break;
        case 0x7D: // LD A,L; 1 byte; 4 cycles
            regA = regL;
            operation_cycles = 4;
            break;
        case 0x7E: // LD A,(HL); 1 byte; 8 cycles
            regA = bus.read(regHL);
            operation_cycles = 8;
            break;
        case 0x7F: // LD A,A; 1 byte; 4 cycles
            operation_cycles = 4;
            break;
        case 0x80: // ADD A,B; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regB, 0);
            operation_cycles = 4;
            break;
        case 0x81: // ADD A,C; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regC, 0);
            operation_cycles = 4;
            break;
        case 0x82: // ADD A,D; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regD, 0);
            operation_cycles = 4;
            break;
        case 0x83: // ADD A,E; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regE, 0);
            operation_cycles = 4;
            break;
        case 0x84: // ADD A,H; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regH, 0);
            operation_cycles = 4;
            break;
        case 0x85: // ADD A,L; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regL, 0);
            operation_cycles = 4;
            break;
        case 0x86: // ADD A,(HL); 1 byte; 8 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, bus.read(regHL), 0);
            operation_cycles = 8;
            break;
        case 0x87: // ADD A,A; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regA, 0);
            operation_cycles = 4;
            break;
        case 0x88: // ADC A,B; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regB, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x89: // ADC A.C; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regC, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x8A: // ADC A,D; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regD, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x8B: // ADC A,E; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regE, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x8C: // ADC A,H; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regH, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x8D: // ADC A,L; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regL, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x8E: // ADC A,(HL); 1 byte; 8 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, bus.read(regHL), flags_reg.flags.C);
            operation_cycles = 8;
            break;
        case 0x8F: // ADC A,A; 1 byte; 4 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, regA, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x90: // SUB B; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regB, 0);
            operation_cycles = 4;
            break;
        case 0x91: // SUB C; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regC, 0);
            operation_cycles = 4;
            break;
        case 0x92: // SUB D; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regD, 0);
            operation_cycles = 4;
            break;
        case 0x93: // SUB E; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regE, 0);
            operation_cycles = 4;
            break;
        case 0x94: // SUB H; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regH, 0);
            operation_cycles = 4;
            break;
        case 0x95: // SUB L; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regL, 0);
            operation_cycles = 4;
            break;
        case 0x96: // SUB (HL); 1 byte; 8 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, bus.read(regHL), 0);
            operation_cycles = 8;
            break;
        case 0x97: // SUB A; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regA, 0);
            operation_cycles = 4;
            break;
        case 0x98: // SBC A,B; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regB, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x99: // SBC A,C; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regC, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x9A: // SBC A,D; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regD, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x9B: // SBC A,E; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regE, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x9C: // SBC A,H; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regH, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x9D: // SBC A,L; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regL, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0x9E: // SBC A,(HL); 1 byte; 8 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, bus.read(regHL), flags_reg.flags.C);
            operation_cycles = 8;
            break;
        case 0x9F: // SBC A,A; 1 byte; 4 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, regA, flags_reg.flags.C);
            operation_cycles = 4;
            break;
        case 0xA0: // AND B; 1 byte; 4 cycles; Z,N,H,C flags
            regA = and8bit_with_flags(regA, regB);
            operation_cycles = 4;
            break;
        case 0xA1: // AND C; 1 byte; 4 cycles; Z,N,H,C flags
            regA = and8bit_with_flags(regA, regC);
            operation_cycles = 4;
            break;
        case 0xA2: // AND D; 1 byte; 4 cycles; Z,N,H,C flags
            regA = and8bit_with_flags(regA, regD);
            operation_cycles = 4;
            break;
        case 0xA3: // AND E; 1 byte; 4 cycles; Z,N,H,C flags
            regA = and8bit_with_flags(regA, regE);
            operation_cycles = 4;
            break;
        case 0xA4: // AND H; 1 byte; 4 cycles; Z,N,H,C flags
            regA = and8bit_with_flags(regA, regH);
            operation_cycles = 4;
            break;
        case 0xA5: // AND L; 1 byte; 4 cycles; Z,N,H,C flags
            regA = and8bit_with_flags(regA, regL);
            operation_cycles = 4;
            break;
        case 0xA6: // AND (HL); 1 byte; 8 cycles; Z,N,H,C flags
            regA = and8bit_with_flags(regA, bus.read(regHL));
            operation_cycles = 8;
            break;
        case 0xA7: // AND A; 1 byte; 4 cycles; Z,N,H,C flags
            regA = and8bit_with_flags(regA, regA);
            operation_cycles = 4;
            break;
        case 0xA8: // XOR B; 1 byte; 4 cycles; Z,N,H,C flags
            regA = xor8bit_with_flags(regA, regB);
            operation_cycles = 4;
            break;
        case 0xA9: // XOR C; 1 byte; 4 cycles; Z,N,H,C flags
            regA = xor8bit_with_flags(regA, regC);
            operation_cycles = 4;
            break;
        case 0xAA: // XOR D; 1 byte; 4 cycles; Z,N,H,C flags
            regA = xor8bit_with_flags(regA, regD);
            operation_cycles = 4;
            break;
        case 0xAB: // XOR E; 1 byte; 4 cycles; Z,N,H,C flags
            regA = xor8bit_with_flags(regA, regE);
            operation_cycles = 4;
            break;
        case 0xAC: // XOR H; 1 byte; 4 cycles; Z,N,H,C flags
            regA = xor8bit_with_flags(regA, regH);
            operation_cycles = 4;
            break;
        case 0xAD: // XOR L; 1 byte; 4 cycles; Z,N,H,C flags
            regA = xor8bit_with_flags(regA, regL);
            operation_cycles = 4;
            break;
        case 0xAE: // XOR M; 1 byte; 8 cycles; Z,N,H,C flags
            regA = xor8bit_with_flags(regA, bus.read(regHL));
            operation_cycles = 8;
            break;
        case 0xAF: // XOR A; 1 byte; 4 cycles; Z,N,H,C flags
            regA = xor8bit_with_flags(regA, regA);
            operation_cycles = 4;
            break;
        case 0xB0: // OR B; 1 byte; 4 cycles; Z,N,H,C flags
            regA = or8bit_with_flags(regA, regB);
            operation_cycles = 4;
            break;
        case 0xB1: // OR C; 1 byte; 4 cycles; Z,N,H,C flags
            regA = or8bit_with_flags(regA, regC);
            operation_cycles = 4;
            break;
        case 0xB2: // OR D; 1 byte; 4 cycles; Z,N,H,C flags
            regA = or8bit_with_flags(regA, regD);
            operation_cycles = 4;
            break;
        case 0xB3: // OR E; 1 byte; 4 cycles; Z,N,H,C flags
            regA = or8bit_with_flags(regA, regE);
            operation_cycles = 4;
            break;
        case 0xB4: // OR H; 1 byte; 4 cycles; Z,N,H,C flags
            regA = or8bit_with_flags(regA, regH);
            operation_cycles = 4;
            break;
        case 0xB5: // OR L; 1 byte; 4 cycles; Z,N,H,C flags
            regA = or8bit_with_flags(regA, regL);
            operation_cycles = 4;
            break;
        case 0xB6: // OR (HL); 1 byte; 8 cycles; Z,N,H,C flags
            regA = or8bit_with_flags(regA, bus.read(regHL));
            operation_cycles = 8;
            break;
        case 0xB7: // OR A; 1 byte; 4 cycles; Z,N,H,C flags
            regA = or8bit_with_flags(regA, regA);
            operation_cycles = 4;
            break;
        case 0xB8: // CMP B; 1 byte; 4 cycles; Z,N,H,C flags
            sub8bit_with_flags(regA, regB, 0);
            operation_cycles = 4;
            break;
        case 0xB9: // CMP C; 1 byte; 4 cycles; Z,N,H,C flags
            sub8bit_with_flags(regA, regC, 0);
            operation_cycles = 4;
            break;
        case 0xBA: // CMP D; 1 byte; 4 cycles; Z,N,H,C flags
            sub8bit_with_flags(regA, regD, 0);
            operation_cycles = 4;
            break;
        case 0xBB: // CMP E; 1 byte; 4 cycles; Z,N,H,C flags
            sub8bit_with_flags(regA, regE, 0);
            operation_cycles = 4;
            break;
        case 0xBC: // CMP H; 1 byte; 4 cycles; Z,N,H,C flags
            sub8bit_with_flags(regA, regH, 0);
            operation_cycles = 4;
            break;
        case 0xBD: // CMP L; 1 byte; 4 cycles; Z,N,H,C flags
            sub8bit_with_flags(regA, regL, 0);
            operation_cycles = 4;
            break;
        case 0xBE: // CMP (HL); 1 byte; 8 cycles; Z,N,H,C flags
            sub8bit_with_flags(regA, bus.read(regHL), 0);
            operation_cycles = 8;
            break;
        case 0xBF: // CMP A; 1 byte; 4 cycles; Z,N,H,C flags
            sub8bit_with_flags(regA, regA, 0);
            operation_cycles = 4;
            break;
        case 0xC0: // RET NZ; 1 byte; 8 cycles
            operation_cycles = cond_return(!flags_reg.flags.Z);
            break;
        case 0xC1: // POP BC; 1 byte; 12 cycles
            regC = stack_pop();
            regB = stack_pop();
            operation_cycles = 12;
            break;
        case 0xC2: // JP NZ,adr; 3 bytes; 12 cycles
            cond_jump(!flags_reg.flags.Z);
            operation_cycles = 12;
            break;
        case 0xC3: // JP adr; 3 bytes; 12 cycles
            regPC = get_next_2_prog_bytes();
            operation_cycles = 12;
            break;
        case 0xC4: // CALL NZ,adr; 3 bytes; 12 cycles
            operation_cycles = cond_call(!flags_reg.flags.Z);
            break;
        case 0xC5: // PUSH BC; 1 byte; 16 cycles
            stack_push(regB);
            stack_push(regC);
            operation_cycles = 16;
            break;
        case 0xC6: // ADD A,n; 2 bytes; 8 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, get_next_prog_byte(), 0);
            operation_cycles = 8;
            break;
        case 0xC7: // RST 00H; 1 byte; 32 cycles
            call_addr(0x0000);
            operation_cycles = 32;
            break;
        case 0xC8: // RET Z; 1 byte; 8 cycles
            operation_cycles = cond_return(flags_reg.flags.Z);
            break;
        case 0xC9: // RET; 1 byte; 8 cycles
            regPC_lower = stack_pop();
            regPC_higher = stack_pop();
            operation_cycles = 8;
            break;
        case 0xCA: // JP Z,adr; 3 bytes; 12 cycles
            cond_jump(flags_reg.flags.Z);
            operation_cycles = 12;
            break;
        case 0xCB: // Extended instructions
        {
            uint8_t op = get_next_prog_byte(); // TODO: Cleanup
            switch (op) {
                case 0x00: // RLC B; 2 bytes; 8 cycles; Z,N,H,C flags
                    regB = rotate_left_with_flags(regB);
                    operation_cycles = 8;
                    break;
                case 0x01: // RLC C; 2 bytes; 8 cycles; Z,N,H,C flags
                    regC = rotate_left_with_flags(regC);
                    operation_cycles = 8;
                    break;
                case 0x02: // RLC D; 2 bytes; 8 cycles; Z,N,H,C flags
                    regD = rotate_left_with_flags(regD);
                    operation_cycles = 8;
                    break;
                case 0x03: // RLC E; 2 bytes; 8 cycles; Z,N,H,C flags
                    regE = rotate_left_with_flags(regE);
                    operation_cycles = 8;
                    break;
                case 0x04: // RLC H; 2 bytes; 8 cycles; Z,N,H,C flags
                    regH = rotate_left_with_flags(regH);
                    operation_cycles = 8;
                    break;
                case 0x05: // RLC L; 2 bytes; 8 cycles; Z,N,H,C flags
                    regL = rotate_left_with_flags(regL);
                    operation_cycles = 8;
                    break;
                case 0x06: // RLC (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    bus.write(regHL, rotate_left_with_flags(bus.read(regHL)));
                    operation_cycles = 16;
                    break;
                case 0x07: // RLC A; 2 bytes; 8 cycles; Z,N,H,C flags
                    regA = rotate_left_with_flags(regA);
                    operation_cycles = 8;
                    break;
                case 0x08: // RRC B; 2 bytes; 8 cycles; Z,N,H,C flags
                    regB = rotate_right_with_flags(regB);
                    operation_cycles = 8;
                    break;
                case 0x09: // RRC C; 2 bytes; 8 cycles; Z,N,H,C flags
                    regC = rotate_right_with_flags(regC);
                    operation_cycles = 8;
                    break;
                case 0x0A: // RRC D; 2 bytes; 8 cycles; Z,N,H,C flags
                    regD = rotate_right_with_flags(regD);
                    operation_cycles = 8;
                    break;
                case 0x0B: // RRC E; 2 bytes; 8 cycles; Z,N,H,C flags
                    regE = rotate_right_with_flags(regE);
                    operation_cycles = 8;
                    break;
                case 0x0C: // RRC H; 2 bytes; 8 cycles; Z,N,H,C flags
                    regH = rotate_right_with_flags(regH);
                    operation_cycles = 8;
                    break;
                case 0x0D: // RRC L; 2 bytes; 8 cycles; Z,N,H,C flags
                    regL = rotate_right_with_flags(regL);
                    operation_cycles = 8;
                    break;
                case 0x0E: // RRC (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    bus.write(regHL, rotate_right_with_flags(bus.read(regHL)));
                    operation_cycles = 16;
                    break;
                case 0x0F: // RRC A; 2 bytes; 8 cycles; Z,N,H,C flags
                    regA = rotate_right_with_flags(regA);
                    operation_cycles = 8;
                    break;
                case 0x10: // RL B; 2 bytes; 8 cycles; Z,N,H,C flags
                    regB = rotate_left_carry_with_flags(regB);
                    operation_cycles = 8;
                    break;
                case 0x11: // RL C; 2 bytes; 8 cycles; Z,N,H,C flags
                    regC = rotate_left_carry_with_flags(regC);
                    operation_cycles = 8;
                    break;
                case 0x12: // RL D; 2 bytes; 8 cycles; Z,N,H,C flags
                    regD = rotate_left_carry_with_flags(regD);
                    operation_cycles = 8;
                    break;
                case 0x13: // RL E; 2 bytes; 8 cycles; Z,N,H,C flags
                    regE = rotate_left_carry_with_flags(regE);
                    operation_cycles = 8;
                    break;
                case 0x14: // RL H; 2 bytes; 8 cycles; Z,N,H,C flags
                    regH = rotate_left_carry_with_flags(regH);
                    operation_cycles = 8;
                    break;
                case 0x15: // RL L; 2 bytes; 8 cycles; Z,N,H,C flags
                    regL = rotate_left_carry_with_flags(regL);
                    operation_cycles = 8;
                    break;
                case 0x16: // RL (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    bus.write(regHL, rotate_left_carry_with_flags(bus.read(regHL)));
                    operation_cycles = 16;
                    break;
                case 0x17: // RL A; 2 bytes; 8 cycles; Z,N,H,C flags
                    regA = rotate_left_carry_with_flags(regA);
                    operation_cycles = 8;
                    break;
                case 0x18: // RR B; 2 bytes; 8 cycles; Z,N,H,C flags
                    regB = rotate_right_carry_with_flags(regB);
                    operation_cycles = 8;
                    break;
                case 0x19: // RR C; 2 bytes; 8 cycles; Z,N,H,C flags
                    regC = rotate_right_carry_with_flags(regC);
                    operation_cycles = 8;
                    break;
                case 0x1A: // RR D; 2 bytes; 8 cycles; Z,N,H,C flags
                    regD = rotate_right_carry_with_flags(regD);
                    operation_cycles = 8;
                    break;
                case 0x1B: // RR E; 2 bytes; 8 cycles; Z,N,H,C flags
                    regE = rotate_right_carry_with_flags(regE);
                    operation_cycles = 8;
                    break;
                case 0x1C: // RR H; 2 bytes; 8 cycles; Z,N,H,C flags
                    regH = rotate_right_carry_with_flags(regH);
                    operation_cycles = 8;
                    break;
                case 0x1D: // RR L; 2 bytes; 8 cycles; Z,N,H,C flags
                    regL = rotate_right_carry_with_flags(regL);
                    operation_cycles = 8;
                    break;
                case 0x1E: // RR (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    bus.write(regHL, rotate_right_carry_with_flags(bus.read(regHL)));
                    operation_cycles = 16;
                    break;
                case 0x1F: // RR A; 2 bytes; 8 cycles; Z,N,H,C flags
                    regA = rotate_right_carry_with_flags(regA);
                    operation_cycles = 8;
                    break;
                case 0x20: // SLA B; 2 bytes; 8 cycles; Z,N,H,C flags
                    regB = shift_left_with_flags(regB);
                    operation_cycles = 8;
                    break;
                case 0x21: // SLA C; 2 bytes; 8 cycles; Z,N,H,C flags
                    regC = shift_left_with_flags(regC);
                    operation_cycles = 8;
                    break;
                case 0x22: // SLA D; 2 bytes; 8 cycles; Z,N,H,C flags
                    regD = shift_left_with_flags(regD);
                    operation_cycles = 8;
                    break;
                case 0x23: // SLA E; 2 bytes; 8 cycles; Z,N,H,C flags
                    regE = shift_left_with_flags(regE);
                    operation_cycles = 8;
                    break;
                case 0x24: // SLA H; 2 bytes; 8 cycles; Z,N,H,C flags
                    regH = shift_left_with_flags(regH);
                    operation_cycles = 8;
                    break;
                case 0x25: // SLA L; 2 bytes; 8 cycles; Z,N,H,C flags
                    regL = shift_left_with_flags(regL);
                    operation_cycles = 8;
                    break;
                case 0x26: // SLA (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    bus.write(regHL, shift_left_with_flags(bus.read(regHL)));
                    operation_cycles = 16;
                    break;
                case 0x27: // SLA A; 2 bytes; 8 cycles; Z,N,H,C flags
                    regA = shift_left_with_flags(regA);
                    operation_cycles = 8;
                    break;
                case 0x28: // SRA B; 2 bytes; 8 cycles; Z,N,H,C flags
                    regB = shift_right_leave_msb_with_flags(regB);
                    operation_cycles = 8;
                    break;
                case 0x29: // SRA C; 2 bytes; 8 cycles; Z,N,H,C flags
                    regC = shift_right_leave_msb_with_flags(regC);
                    operation_cycles = 8;
                    break;
                case 0x2A: // SRA D; 2 bytes; 8 cycles; Z,N,H,C flags
                    regD = shift_right_leave_msb_with_flags(regD);
                    operation_cycles = 8;
                    break;
                case 0x2B: // SRA E; 2 bytes; 8 cycles; Z,N,H,C flags
                    regE = shift_right_leave_msb_with_flags(regE);
                    operation_cycles = 8;
                    break;
                case 0x2C: // SRA H; 2 bytes; 8 cycles; Z,N,H,C flags
                    regH = shift_right_leave_msb_with_flags(regH);
                    operation_cycles = 8;
                    break;
                case 0x2D: // SRA L; 2 bytes; 8 cycles; Z,N,H,C flags
                    regL = shift_right_leave_msb_with_flags(regL);
                    operation_cycles = 8;
                    break;
                case 0x2E: // SRA (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    bus.write(regHL, shift_right_leave_msb_with_flags(bus.read(regHL)));
                    operation_cycles = 16;
                    break;
                case 0x2F: // SRA A; 2 bytes; 8 cycles; Z,N,H,C flags
                    regA = shift_right_leave_msb_with_flags(regA);
                    operation_cycles = 8;
                    break;
                case 0x30: // SWAP B; 2 bytes; 8 cycles; Z,N,H,C flags
                    regB = swap_nibbles_with_flags(regB);
                    operation_cycles = 8;
                    break;
                case 0x31: // SWAP C; 2 bytes; 8 cycles; Z,N,H,C flags
                    regC = swap_nibbles_with_flags(regC);
                    operation_cycles = 8;
                    break;
                case 0x32: // SWAP D; 2 bytes; 8 cycles; Z,N,H,C flags
                    regD = swap_nibbles_with_flags(regD);
                    operation_cycles = 8;
                    break;
                case 0x33: // SWAP E; 2 bytes; 8 cycles; Z,N,H,C flags
                    regE = swap_nibbles_with_flags(regE);
                    operation_cycles = 8;
                    break;
                case 0x34: // SWAP H; 2 bytes; 8 cycles; Z,N,H,C flags
                    regH = swap_nibbles_with_flags(regH);
                    operation_cycles = 8;
                    break;
                case 0x35: // SWAP L; 2 bytes; 8 cycles; Z,N,H,C flags
                    regL = swap_nibbles_with_flags(regL);
                    operation_cycles = 8;
                    break;
                case 0x36: // SWAP (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    bus.write(regHL, swap_nibbles_with_flags(bus.read(regHL)));
                    operation_cycles = 16;
                    break;
                case 0x37: // SWAP A; 2 bytes; 8 cycles; Z,N,H,C flags
                    regA = swap_nibbles_with_flags(regA);
                    operation_cycles = 8;
                    break;
                case 0x38: // SRL B; 2 bytes; 8 cycles; Z,N,H,C flags
                    regB = shift_right_with_flags(regB);
                    operation_cycles = 8;
                    break;
                case 0x39: // SRL C; 2 bytes; 8 cycles; Z,N,H,C flags
                    regC = shift_right_with_flags(regC);
                    operation_cycles = 8;
                    break;
                case 0x3A: // SRL D; 2 bytes; 8 cycles; Z,N,H,C flags
                    regD = shift_right_with_flags(regD);
                    operation_cycles = 8;
                    break;
                case 0x3B: // SRL E; 2 bytes; 8 cycles; Z,N,H,C flags
                    regE = shift_right_with_flags(regE);
                    operation_cycles = 8;
                    break;
                case 0x3C: // SRL H; 2 bytes; 8 cycles; Z,N,H,C flags
                    regH = shift_right_with_flags(regH);
                    operation_cycles = 8;
                    break;
                case 0x3D: // SRL L; 2 bytes; 8 cycles; Z,N,H,C flags
                    regL = shift_right_with_flags(regL);
                    operation_cycles = 8;
                    break;
                case 0x3E: // SRL (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    bus.write(regHL, shift_right_with_flags(bus.read(regHL)));
                    operation_cycles = 16;
                    break;
                case 0x3F: // SRL A; 2 bytes; 8 cycles; Z,N,H,C flags
                    regA = shift_right_with_flags(regA);
                    operation_cycles = 8;
                    break;
                default: // BIT, SET, RES operations // TODO: Cleanup
                {
                    int bit_no = (op & 0b00111000) >> 3;
                    int reg_id = op & 0b00000111;
                    uint8_t *reg_lookup[] = {&regB, &regC, &regD, &regE, &regH, &regL};
                    switch((op & 0b11000000) >> 6) {
                        case 0b01: // BIT b,r; 2 bytes; 8/16 cycles; Z,N,H flags
                            if (reg_id == 7) {
                                test_bit_with_flags(bit_no, regA);
                                operation_cycles = 8;
                            } else if (reg_id == 6) {
                                test_bit_with_flags(bit_no, bus.read(regHL));
                                operation_cycles = 16;
                            } else {
                                test_bit_with_flags(bit_no, *reg_lookup[reg_id]);
                                operation_cycles = 8;
                            }
                            break;
                        case 0b10: // RES b,r; 2 bytes; 8/16 cycles
                            if (reg_id == 7) {
                                regA = regA & (~(1 << bit_no));
                                operation_cycles = 8;
                            } else if (reg_id == 6) {
                                bus.write(regHL, bus.read(regHL) & (~(1 << bit_no)));
                                operation_cycles = 16;
                            } else {
                                *reg_lookup[reg_id] = *reg_lookup[reg_id] & (~(1 << bit_no));
                                operation_cycles = 8;
                            }
                            break;
                        case 0b11: // SET b,r; 2 bytes; 8/16 cycles
                            if (reg_id == 7) {
                                regA = regA | (1 << bit_no);
                                operation_cycles = 8;
                            } else if (reg_id == 6) {
                                bus.write(regHL, bus.read(regHL) | (1 << bit_no));
                                operation_cycles = 16;
                            } else {
                                *reg_lookup[reg_id] = *reg_lookup[reg_id] | (1 << bit_no);
                                operation_cycles = 8;
                            }
                            break;
                    }
                }
                    break;
            }
        }
            break;
        case 0xCC: // CALL Z,adr; 3 bytes; 12 cycles
            operation_cycles = cond_call(flags_reg.flags.Z);
            break;
        case 0xCD: // CALL adr; 3 bytes; 12 cycles
            {
                uint16_t newPC = get_next_2_prog_bytes();
                stack_push(regPC_higher);
                stack_push(regPC_lower);
                regPC = newPC;
            }
            operation_cycles = 12;
            break;
        case 0xCE: // ADC A,n; 2 bytes; 8 cycles; Z,N,H,C flags
            regA = add8bit_with_flags(regA, get_next_prog_byte(), flags_reg.flags.C);
            operation_cycles = 8;
            break;
        case 0xCF: // RST 08H; 1 byte; 32 cycles
            call_addr(0x0008);
            operation_cycles = 32;
            break;
        case 0xD0: // RET NC; 1 byte; 8 cycles
            operation_cycles = cond_return(!flags_reg.flags.C);
            break;
        case 0xD1: // POP DE; 1 byte; 12 cycles
            regE = stack_pop();
            regD = stack_pop();
            operation_cycles = 12;
            break;
        case 0xD2: // JP NC,adr; 3 bytes; 12 cycles
            cond_jump(!flags_reg.flags.C);
            operation_cycles = 12;
            break;
        // // TODO: To update
        // case 0xD3: // OUT D8; 2 bytes; 10 cycles
        //     io_write(get_next_prog_byte(), regA);
        //     operation_cycles = 10;
        //     break;
        case 0xD4: // CALL NC,adr; 3 bytes; 12 cycles
            operation_cycles = cond_call(!flags_reg.flags.C);
            break;
        case 0xD5: // PUSH DE; 1 byte; 16 cycles
            stack_push(regD);
            stack_push(regE);
            operation_cycles = 16;
            break;
        case 0xD6: // SUB n; 2 bytes; 8 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, get_next_prog_byte(), 0);
            operation_cycles = 8;
            break;
        case 0xD7: // RST 10H; 1 byte; 32 cycles
            call_addr(0x0010);
            operation_cycles = 32;
            break;
        case 0xD8: // RET C; 1 byte; 8 cycles
            operation_cycles = cond_return(flags_reg.flags.C);
            break;
        case 0xD9: // RETI; 1 byte; 8 cycles
            regPC_lower = stack_pop();
            regPC_higher = stack_pop();
            bus.io.interrupts.all_interrupts_enable();
            operation_cycles = 8;
            break;
        case 0xDA: // JP C,adr; 3 bytes; 12 cycles
            cond_jump(flags_reg.flags.C);
            operation_cycles = 12;
            break;
        // // TODO: To update
        // case 0xDB: // IN D8; 2 bytes; 10 cycles
        //     regA = io_read(get_next_prog_byte());
        //     operation_cycles = 10;
        //     break;
        case 0xDC: // CALL C,adr; 3 bytes; 12 cycles
            operation_cycles = cond_call(flags_reg.flags.C);
            break;
        // // TODO: To update
        // case 0xDD: // - (works as CALL addr); 3 bytes; 17 cycles
        //     {
        //         uint16_t newPC = get_next_2_prog_bytes();
        //         stack_push(regPC_higher);
        //         stack_push(regPC_lower);
        //         regPC = newPC;
        //     }
        //     operation_cycles = 17;
        //     break;
        case 0xDE: // SBC A,n; 2 bytes; 8 cycles; Z,N,H,C flags
            regA = sub8bit_with_flags(regA, get_next_prog_byte(), flags_reg.flags.C);
            operation_cycles = 8;
            break;
        case 0xDF: // RST 18H; 1 byte; 32 cycles
            call_addr(0x0018);
            operation_cycles = 32;
            break;
        case 0xE0: // LD ($FF00 + n),A; 2 bytes; 12 cycles
            bus.write(0xFF00 + get_next_prog_byte(), regA);
            operation_cycles = 12;
            break;
        case 0xE1: // POP HL; 1 byte; 12 cycles
            regL = stack_pop();
            regH = stack_pop();
            operation_cycles = 12;
            break;
        case 0xE2: // LD ($FF00 + C),A; 1 byte; 8 cycles
            bus.write(0xFF00 + regC, regA);
            operation_cycles = 8;
            break;
        // // TODO: To update
        // case 0xE3: // XTHL; 1 byte; 18 cycles
        //     {
        //         uint8_t tmp = regL;
        //         regL = bus.read(regSP);
        //         bus.write(regSP, tmp);
        //         tmp = regH;
        //         regH = bus.read(regSP+1);
        //         bus.write(regSP+1, tmp);
        //     }
        //     operation_cycles = 18;
        //     break;
        // // TODO: To update
        // case 0xE4: // CPO adr; 3 bytes; 17/11 cycles
        //     operation_cycles = cond_call(!flags_reg.flags.P);
        //     break;
        case 0xE5: // PUSH HL; 1 byte; 16 cycles
            stack_push(regH);
            stack_push(regL);
            operation_cycles = 16;
            break;
        case 0xE6: // AND n; 2 bytes; 8 cycles; Z,N,H,C flags
            regA = and8bit_with_flags(regA, get_next_prog_byte());
            operation_cycles = 8;
            break;
        case 0xE7: // RST 20H; 1 byte; 32 cycles
            call_addr(0x0020);
            operation_cycles = 32;
            break;
        case 0xE8: // ADD SP,n; 2 bytes; 16 cycles; Z,N,H,C flags
            regSP = add_s8bit_to_u16bit_with_flags(unsigned_byte_to_signed(get_next_prog_byte()), regSP);
            break;
        case 0xE9: // JP (HL); 1 byte; 4 cycles
            regPC = regHL; // TODO: Check if this is correct
            operation_cycles = 4;
            break;
        case 0xEA: // LD (nn),A; 3 bytes; 16 cycles
            bus.write(get_next_2_prog_bytes(), regA);
            operation_cycles = 16;
            break;
        // // TODO: To update
        // case 0xEB: // XCHG; 1 byte; 5 cycles
        //     {
        //         uint16_t tmp = regHL;
        //         regHL = regDE;
        //         regDE = tmp;
        //     }
        //     operation_cycles = 5;
        //     break;
        // // TODO: To update
        // case 0xEC: // CPE adr; 3 bytes; 17/11 cycles
        //     operation_cycles = cond_call(flags_reg.flags.P);
        //     break;
        // // TODO: To update
        // case 0xED: // - (works as CALL addr); 3 bytes; 17 cycles
        //     {
        //         uint16_t newPC = get_next_2_prog_bytes();
        //         stack_push(regPC_higher);
        //         stack_push(regPC_lower);
        //         regPC = newPC;
        //     }
        //     operation_cycles = 17;
        //     break;
        case 0xEE: // XOR n; 2 bytes; 8 cycles; Z,N,H,C flags
            regA = xor8bit_with_flags(regA, get_next_prog_byte());
            operation_cycles = 8;
            break;
        case 0xEF: // RST 28H; 1 byte; 32 cycles
            call_addr(0x0028);
            operation_cycles = 32;
            break;
        case 0xF0: // LD A,($FF00 + n); 2 bytes; 12 cycles
            regA = bus.read(0xFF00 + get_next_prog_byte());
            operation_cycles = 12;
            break;
        case 0xF1: // POP AF; 1 byte; 12 cycles
            flags_reg.value = stack_pop();
            regA = stack_pop();
            // Unused flags should always be 0
            flags_reg.flags._unused = 0;
            operation_cycles = 12;
            break;
        case 0xF2: // LD A,($FF00 + C); 1 byte; 8 cycles
            regA = bus.read(0xFF00 + regC);
            operation_cycles = 8;
            break;
        case 0xF3: // DI; 1 byte; 4 cycles
            bus.io.interrupts.all_interrupts_disable();
            operation_cycles = 4;
            break;
        // // TODO: To update
        // case 0xF4: // CP adr; 3 bytes; 17/11 cycles
        //     operation_cycles = cond_call(!flags_reg.flags.S); // If the number is positive
        //     break;
        case 0xF5: // PUSH AF; 1 byte; 16 cycles
            stack_push(regA);
            stack_push(flags_reg.value);
            operation_cycles = 16;
            break;
        case 0xF6: // OR n; 2 bytes; 8 cycles; Z,N,H,C flags
            regA = or8bit_with_flags(regA, get_next_prog_byte());
            operation_cycles = 8;
            break;
        case 0xF7: // RST 30H; 1 byte; 32 cycles
            call_addr(0x0030);
            operation_cycles = 32;
            break;
        case 0xF8: // LDHL SP+n; 2 bytes; 12 cycles; Z,N,H,C flags
            regHL = add_s8bit_to_u16bit_with_flags(unsigned_byte_to_signed(get_next_prog_byte()), regSP);
            operation_cycles = 12;
            break;
        case 0xF9: // LD SP,HL; 1 byte; 8 cycles
            regSP = regHL;
            operation_cycles = 8;
            break;
        case 0xFA: // LD A,(nn); 3 bytes; 16 cycles
            regA = bus.read(get_next_2_prog_bytes());
            operation_cycles = 16;
            break;
        case 0xFB: // EI; 1 byte; 4 cycles
            bus.io.interrupts.order_all_intrs_enable();
            operation_cycles = 4;
            break;
        // // TODO: To update
        // case 0xFC: // CM adr; 3 bytes; 17/11 cycles
        //     operation_cycles = cond_call(flags_reg.flags.S); // If the number is negative
        //     break;
        // // TODO: To update
        // case 0xFD: // - (works as CALL addr); 3 bytes; 17 cycles
        //     {
        //         uint16_t newPC = get_next_2_prog_bytes();
        //         stack_push(regPC_higher);
        //         stack_push(regPC_lower);
        //         regPC = newPC;
        //     }
        //     operation_cycles = 17;
        //     break;
        case 0xFE: // CMP n; 2 bytes; 8 cycles; Z,N,H,C flags
            sub8bit_with_flags(regA, get_next_prog_byte(), 0); // TODO: Check what's wrong with H flag
            operation_cycles = 8;
            break;
        case 0xFF: // RST 38H; 1 byte; 32 cycles
            call_addr(0x0038);
            operation_cycles = 32;
            break;
        default:
            break;
    }
    return operation_cycles;
}
