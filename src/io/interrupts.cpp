#include "io/interrupts.h"

Interrupts::Interrupts() {
    interrupt_flag.value = 0xE1;
    interrupt_enable.value = 0x00;
    intrs_should_be_enabled = false;
}

Interrupts::~Interrupts() {

}

intr_type_t Interrupts::get_ready_interrupt() {
    for (int bit_no = 0; bit_no <= 4; ++bit_no) {
        if (((interrupt_enable.value & interrupt_flag.value) & bit_no) != 0) {
            return static_cast<intr_type_t>(bit_no);
        }
    }
    return NO_INTERRUPT;
}

void Interrupts::order_all_intrs_enable() {
    intrs_should_be_enabled = true;
}

void Interrupts::all_interrupts_enable() {
    interrupt_enable.value = 0xFF;
}

void Interrupts::all_interrupts_disable() {
    interrupt_enable.value = 0x00;
}

void Interrupts::intrs_update_state(bool state_before_last_opcode) {
    if (state_before_last_opcode) {
        interrupt_enable.value = 0xFF;
    }
    intrs_should_be_enabled = false;
}

bool Interrupts::get_intrs_should_be_enabled() {
    return intrs_should_be_enabled;
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
