#include <cstring>
#include "io/joypad.h"

Joypad::Joypad() {
    // TODO: What about those unused bits?
    memset(btn_states, NOT_PRESSED, 8 * sizeof(btn_state_t));
}

Joypad::~Joypad() {

}

void Joypad::attach_interrupts_handler(Interrupts *interrupts) {
    intr = interrupts;
}

void Joypad::btn_change_state(btn_type_t button, btn_state_t new_state) {
    if (new_state == PRESSED && btn_states[button] == NOT_PRESSED) {
        /* TODO: Interrupt shouldn't occur if there is no HIGH-LOW transition on the line
        for instance if A button is already pressed and user presses RIGHT.
        It's not very important quirk so I'll implement it later or maybe I won't
        */
        intr->signal(intr_type_t::JOYPAD);
    }
    btn_states[button] = new_state;
}

uint8_t Joypad::get_data_reg_val() {
    if (data_reg.bits.select_action_btns == btn_select_t::SELECTED) {
        data_reg.bits.right_or_A = btn_states[btn_type_t::A];
        data_reg.bits.left_or_B = btn_states[btn_type_t::B];
        data_reg.bits.up_or_select = btn_states[btn_type_t::SELECT];
        data_reg.bits.down_or_start = btn_states[btn_type_t::START];
    } else if (data_reg.bits.select_direct_btns == btn_select_t::SELECTED) {
        data_reg.bits.right_or_A = btn_states[btn_type_t::RIGHT];
        data_reg.bits.left_or_B = btn_states[btn_type_t::LEFT];
        data_reg.bits.up_or_select = btn_states[btn_type_t::UP];
        data_reg.bits.down_or_start = btn_states[btn_type_t::DOWN];
    } else {
        // Neither selected - no button pressed
        data_reg.value = 0xFF; // TODO: Set correct value of unused bits
    }
    return data_reg.value;
}

void Joypad::set_data_reg_val(uint8_t value) {
    data_reg.value = value;
}
