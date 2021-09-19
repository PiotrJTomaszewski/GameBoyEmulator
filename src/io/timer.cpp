#include "io/timer.h"

// TODO: Implement timer obscure behaviour - https://gbdev.io/pandocs/Timer_Obscure_Behaviour.html
Timer::Timer() {
    DIV_CPU_clock_counter = 0;
    TIMA_CPU_clock_counter = 0;
    is_DIV_stopped = false;
}

Timer::~Timer() {

}

void Timer::attach_interrupts_handler(Interrupts *interrupts) {
    this->interrupts = interrupts; // TODO: Maybe do it differently
}

void Timer::tick(unsigned cpu_cycles) {
    if (!is_DIV_stopped) {
        DIV_CPU_clock_counter += cpu_cycles;
        // DIV is incremented at a rate of 16384Hz which is equal to 256 CPU clock cycles
        if (DIV_CPU_clock_counter >= 256) {
            timer_data.DIV = (timer_data.DIV + 1) & 0xFF;
        }
        DIV_CPU_clock_counter %= 256;
    }

    if (timer_data.TAC.mode.timer_enabled) {
        TIMA_CPU_clock_counter += cpu_cycles;
        if (TIMA_CPU_clock_counter >= CLK_DIVIDER_LOOKUP[timer_data.TAC.mode.clk_divider]) {
            // Timer will overflow when incremented
            if (timer_data.TIMA == 0xFF) {
                timer_data.TIMA = timer_data.TMA;
                interrupts->signal(intr_type_t::TIMER);
            } else {
                ++timer_data.TIMA;
            }
            TIMA_CPU_clock_counter %= CLK_DIVIDER_LOOKUP[timer_data.TAC.mode.clk_divider];
        }
    }
}

void Timer::stop_DIV() {
    reset_DIV_counter();
    is_DIV_stopped = true;
}

void Timer::run_DIV_after_stop() {
    is_DIV_stopped = false;
}

void Timer::write(uint16_t address, uint8_t value) {
    switch (address) {
        case 0xFF04:
            timer_data.DIV = value;
            break;
        case 0xFF05:
            timer_data.TIMA = value;
            break;
        case 0xFF06:
            timer_data.TMA = value;
            break;
        case 0xFF07:
            timer_data.TAC.value = value;
            break;
        default:
            break;
    }
}

uint8_t Timer::read(uint16_t address) {
    uint8_t value = 0;
    switch (address) {
        case 0xFF04:
            value = timer_data.DIV;
            break;
        case 0xFF05:
            value = timer_data.TIMA = value;
            break;
        case 0xFF06:
            value = timer_data.TMA;
            break;
        case 0xFF07:
            value = timer_data.TAC.value;
            break;
        default:
            break;
    }
    return value;
}

inline void Timer::reset_DIV_counter() {
    timer_data.DIV = 0;
    DIV_CPU_clock_counter = 0;
}

uint8_t Timer::get_DIV() {
    return timer_data.DIV;
}

uint8_t Timer::get_TIMA() {
    return timer_data.TIMA;
}

uint8_t Timer::get_TMA() {
    return timer_data.TMA;
}

bool Timer::get_TAC_is_enabled() {
    return timer_data.TAC.mode.timer_enabled;
}

unsigned Timer::get_TAC_clock_divider() {
    return CLK_DIVIDER_LOOKUP[timer_data.TAC.mode.clk_divider];
}
