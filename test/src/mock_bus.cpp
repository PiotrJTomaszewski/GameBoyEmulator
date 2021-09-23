#include <fstream>
#include <stdexcept>
#include <cstring>
#include "mock_bus.h"

MockBus::MockBus() {
    log_serial = false;
}

MockBus::MockBus(bool log_serial) {
    this->log_serial = log_serial;
    if (log_serial) {
        memset(serial_data, 0, sizeof(serial_data));
        serial_data_newline_count = 0;
        serial_data_last_char_index = 0;
    }
}

MockBus::~MockBus() {

}

void MockBus::write(uint16_t address, uint8_t value) {
    data[address] = value;
    if (log_serial && address == 0xFF02 && value == 0x81) { // Starting serial transfer
        serial_data[serial_data_last_char_index++] = data[0xFF01]; // Storing value of serial transfer data register
        data[0xFF02] = 0x01; // Marking byte as recieved
        if (data[0xFF01] == '\n') {
            ++serial_data_newline_count;
        }
    }
}

uint8_t MockBus::read(uint16_t address) {
    return data[address];
}


void MockBus::force_write(uint16_t address, uint8_t value) {
    data[address] = value;
}

void MockBus::load_file(std::string path) {
    int file_size;
    std::ifstream file(path, std::ios::binary);
    if (!file.good()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    file_size = static_cast<int>(file.tellg());
    file.seekg(0, std::ios::end);
    file_size = static_cast<int>(file.tellg()) - file_size;

    file.seekg(0, std::ios::beg);
    file.read((char *)(data), file_size);
    file.close();
}

char *MockBus::get_serial_data_log() {
    return serial_data;
}

int MockBus::get_serial_data_newline_count() {
    return serial_data_newline_count;
}
