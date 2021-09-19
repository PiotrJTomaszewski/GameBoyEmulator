#pragma once
#include <cstdint>
#include "read_write_interface.h"
#include "io/interrupts.h"

class Timer: public ReadWriteInterface {
public:
    Timer();
    ~Timer();
    void attach_interrupts_handler(Interrupts *interrupts);
    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
    void tick(unsigned cpu_cycles);
    void stop_DIV();
    void run_DIV_after_stop();
    uint8_t get_DIV();
    uint8_t get_TIMA();
    uint8_t get_TMA();
    bool get_TAC_is_enabled();
    unsigned get_TAC_clock_divider();

private:
enum TAC_clk_speed_t {
    DIV_1024 = 0b00,
    DIV_16 = 0b01,
    DIV_64 = 0b10,
    DIV_256 = 0b11
};

const unsigned CLK_DIVIDER_LOOKUP[4] = {1024, 16, 64, 256};

union __attribute__((packed)) timer_ctrl_t {
    uint8_t value;
    struct __attribute__((packed)) INTERNAL {
        TAC_clk_speed_t clk_divider: 2;
        bool timer_enabled: 1;
        unsigned _unused: 5;
    } mode;
};

struct __attribute__((packed)) timer_t {
    uint8_t DIV;
    uint8_t TIMA;
    uint8_t TMA;
    timer_ctrl_t TAC; 
} timer_data;

Interrupts *interrupts;

bool is_DIV_stopped;
unsigned DIV_CPU_clock_counter;
unsigned TIMA_CPU_clock_counter;
inline void reset_DIV_counter();
};
