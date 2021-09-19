#include "io/io.h"

IO::IO() {
    timer.attach_interrupts_handler(&interrupts);
}

IO::~IO() {

}

void IO::write(uint16_t address, uint8_t value) {
    if (address == 0xFF0F) { // Interrupt Flag
        interrupts.interrupt_flag.value = value;
    } else if (address == 0xFFFF) { // Interrupt Enable
        interrupts.interrupt_enable.value = value;
    } else if (address >= 0xFF04 && address <= 0xFF07) {
        timer.write(address, value);
    } else {
        data[address-0xFF00] = value;
    }
}

uint8_t IO::read(uint16_t address) {
    uint8_t value = 0;
    if (address == 0xFF0F) { // Interrupt Flag
        value = interrupts.interrupt_flag.value;
    } else if (address == 0xFFFF) { // Interrupt Enable
        value = interrupts.interrupt_enable.value;
    } else if (address >= 0xFF04 && address <= 0xFF07) {
        value = timer.read(address);
    } else {
        value = data[address-0xFF00];
    }
    return value;
}
