#include "io/interrupts.h"

Interrupts::Interrupts() {
    interrupt_flag.value = 0xE1;
    interrupt_enable.value = 0x00;
    IME_flag = false;
    is_IME_flag_enabling_scheduled = false;
}

Interrupts::~Interrupts() {

}

bool Interrupts::is_interrupt_pending() {
    return ((interrupt_enable.value & interrupt_flag.value) != 0);
}

intr_type_t Interrupts::get_ready_interrupt() {
    if (IME_flag) {
        for (int bit_no = 0; bit_no <= 4; ++bit_no) {
            if (((interrupt_enable.value & interrupt_flag.value) & (1 << bit_no)) != 0) {
                return static_cast<intr_type_t>(bit_no);
            }
        }
    }
    return NO_INTERRUPT;
}

void Interrupts::order_all_intrs_enable() {
    is_IME_flag_enabling_scheduled = true;
}

void Interrupts::enable_IME_flag() {
    IME_flag = true;
}

void Interrupts::disable_IME_flag() {
    IME_flag = false;
}

void Interrupts::IME_flag_update_state(bool state_before_last_opcode) {
    if (state_before_last_opcode) {
        IME_flag = true;
        is_IME_flag_enabling_scheduled = false;
    }
}

bool Interrupts::get_is_IME_flag_enabling_scheduled() {
    return is_IME_flag_enabling_scheduled;
}

void Interrupts::signal(intr_type_t type) {
    switch (type) {
        case VBLANK:
            interrupt_flag.flags.vblank = 1;
            break;
        case LCD_STAT:
            interrupt_flag.flags.lcd_stat = 1;
            break;
        case TIMER:
            interrupt_flag.flags.timer = 1;
            break;
        case SERIAL:
            interrupt_flag.flags.serial = 1;
            break;
        case JOYPAD:
            interrupt_flag.flags.joypad = 1;
            break;
        default:
            break;
    }
}

void Interrupts::mark_used(intr_type_t type) {
    switch (type) {
        case VBLANK:
            interrupt_flag.flags.vblank = 0;
            break;
        case LCD_STAT:
            interrupt_flag.flags.lcd_stat = 0;
            break;
        case TIMER:
            interrupt_flag.flags.timer = 0;
            break;
        case SERIAL:
            interrupt_flag.flags.serial = 0;
            break;
        case JOYPAD:
            interrupt_flag.flags.joypad = 0;
            break;
        default:
            break;
    }
}
