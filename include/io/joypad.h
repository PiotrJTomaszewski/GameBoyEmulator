#pragma once
#include <cstdint>
#include "io/interrupts.h"

class Joypad {
public:
    Joypad();
    ~Joypad();
    enum btn_type_t {
        RIGHT = 0,
        LEFT,
        UP,
        DOWN,
        A,
        B,
        START,
        SELECT
    };

    enum btn_state_t {
        PRESSED = 0,
        NOT_PRESSED = 1
    };

    void btn_change_state(btn_type_t button, btn_state_t new_state);
    uint8_t get_data_reg_val();
    void set_data_reg_val(uint8_t value);
    void attach_interrupts_handler(Interrupts *interrupts);

private:
    enum btn_select_t {
        SELECTED = 0,
        NOT_SELECTED = 1
    };

    union __attribute__((packed)) joypad_data_t {
        uint8_t value;
        struct __attribute__((packed)) INNER {
            btn_state_t right_or_A: 1;
            btn_state_t left_or_B: 1;
            btn_state_t up_or_select: 1;
            btn_state_t down_or_start: 1;
            btn_select_t select_direct_btns: 1;
            btn_select_t select_action_btns: 1;
            unsigned _unused: 2;
        } bits;
    };
    joypad_data_t data_reg;
    btn_state_t btn_states[8];
    Interrupts *intr;
};
