#pragma once
#include <cstdint>
#include <string>

// Starting at 'Cartridge Type' field in the header. The ones before aren't interesting to me (at least for now). So the ones after RAM size
#define CARTRIDGE_HEADER_START 0x147
#define CARTRIDGE_HEADER_SIZE 3

// Info from https://gbdev.io/pandocs/The_Cartridge_Header.html#the-cartridge-header
enum cartridge_type_t {
    ROM_ONLY = 0x00,
    MBC1 = 0x01,
    MBC1_RAM = 0x02,
    MBC1_RAM_BATTERY = 0x03,
    MBC2 = 0x05,
    MBC2_BATTERY = 0x06,
    ROM_RAM = 0x08, // Unused
    ROM_RAM_BATTERY = 0x09, // Unused
    MMM01 = 0x0B,
    MMM01_RAM = 0x0C,
    MMM01_RAM_BATTERY = 0x0D,
    MBC3_TIMER_BATTERY = 0x0F,
    MBC3_TIMER_RAM_BATTERY = 0x10,
    MBC3 = 0x11,
    MBC3_RAM = 0x12,
    MBC3_RAM_BATTERY = 0x13,
    MBC5 = 0x19,
    MBC5_RAM = 0x1A,
    MBC5_RAM_BATTERY = 0x1B,
    MBC5_RUMBLE = 0x1C,
    MBC5_RUMBLE_RAM = 0x1D,
    MBC5_RUMBLE_RAM_BATTERY = 0x1E,
    MBC6 = 0x20,
    MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
    POCKET_CAMERA = 0xFC,
    BANDAI_TAMA5 = 0xFD,
    HuC3 = 0xFE,
    HuC1_RAM_BATTERY = 0xFF
};

struct __attribute__((packed)) cardridge_header_t {
    cartridge_type_t type: 8; // 0x0147
    uint8_t ROM_size_shift; // 0x0148 - actual size is 32kB << ROM_size_shift
    uint8_t RAM_size_id; // 0x0149 - here is not as straightforward as above so I have to hardcode it
};

std::string get_cartridge_type_name(cartridge_type_t type);
