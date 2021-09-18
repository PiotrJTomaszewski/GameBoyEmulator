#pragma once
#include <cstdint>

struct __attribute__((packed)) intr_reg_t {
    uint8_t value;
    struct FLAGS {
        unsigned vblank: 1;
        unsigned lcd_stat: 1;
        unsigned timer: 1;
        unsigned serial: 1;
        unsigned joypad: 1;
        unsigned _unused: 3;
    } flags;
};

enum intr_type_t {
    NO_INTERRUPT = -1,
    VBLANK = 0,
    LCD_STAT = 1,
    TIMER = 2,
    SERIAL = 3,
    JOYPAD = 4
};

class Interrupts {
friend class IO;
public:
    Interrupts();
    ~Interrupts();
    intr_type_t get_ready_interrupt();
    /**
     * Interrupts will be enabled after the next instruction executes
     * Because EI instruction has delay of one cycle
     */
    void order_all_intrs_enable();
    void all_interrupts_enable();
    void all_interrupts_disable();
    void intrs_update_state(bool state_before_last_opcode);
    bool get_intrs_should_be_enabled();
    void signal(intr_type_t type);
    void mark_used(intr_type_t type);

private:
    intr_reg_t interrupt_flag;
    intr_reg_t interrupt_enable;
    bool intrs_should_be_enabled;
};
