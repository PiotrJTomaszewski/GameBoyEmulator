#include "mock_bus.h"

MockBus::MockBus() {

}

MockBus::~MockBus() {

}

void MockBus::write(uint16_t address, uint8_t value) {
    data[address] = value;
}

uint8_t MockBus::read(uint16_t address) {
    return data[address];
}


void MockBus::force_write(uint16_t address, uint8_t value) {
    data[address] = value;
}
