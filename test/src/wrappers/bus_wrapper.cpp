#include "wrappers/bus_wrapper.h"

BusWrapper::BusWrapper(bool log_serial) {
    this->log_serial = log_serial;
}

BusWrapper::~BusWrapper() {

}

void BusWrapper::write(uint16_t address, uint8_t value) {
    Bus::write(address, value);
    if (log_serial && address == 0xFF02 && value == 0x81) { // Starting serial transfer
        serial_data[serial_data_last_char_index++] = io.read(0xFF01); // Storing value of serial transfer data register
        io.write(0xFF02, 0x01); // Marking byte as recieved
        if (io.read(0xFF01) == '\n') {
            ++serial_data_newline_count;
        }
    }
}

char *BusWrapper::get_serial_data_log() {
    return serial_data;
}

int BusWrapper::get_serial_data_newline_count() {
    return serial_data_newline_count;
}
