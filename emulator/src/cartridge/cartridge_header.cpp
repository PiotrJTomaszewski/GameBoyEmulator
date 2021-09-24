#include "cartridge/cartridge_header.h"

std::string get_cartridge_type_name(cartridge_type_t type) {
    switch (type) {
        case ROM_ONLY:
            return "ROM ONLY";
        case MBC1:
            return "MBC1";
        case MBC1_RAM:
            return "MBC1+RAM";
        case MBC1_RAM_BATTERY:
            return "MBC1+RAM+BATTERY";
        case MBC2:
            return "MBC2";
        case MBC2_BATTERY:
            return "MBC2+BATTERY";
        case ROM_RAM:
            return "ROM+RAM";
        case ROM_RAM_BATTERY:
            return "ROM+RAM+BATTERY";
        case MMM01:
            return "MMM01";
        case MMM01_RAM:
            return "MMM01+RAM";
        case MMM01_RAM_BATTERY:
            return "MMM01+RAM+BATTERY";
        case MBC3_TIMER_BATTERY:
            return "MBC3+TIMER+BATTERY";
        case MBC3_TIMER_RAM_BATTERY:
            return "MBC3+TIMER+BATTERY";
        case MBC3:
            return "MBC3";
        case MBC3_RAM:
            return "MBC3+RAM";
        case MBC3_RAM_BATTERY:
            return "MBC3+RAM+BATTERY";
        case MBC5:
            return "MBC5";
        case MBC5_RAM:
            return "MBC5+RAM";
        case MBC5_RAM_BATTERY:
            return "MBC5+RAM+BATTERY";
        case MBC5_RUMBLE:
            return "MBC5+RUMBLE";
        case MBC5_RUMBLE_RAM:
            return "MBC5+RUMBLE+RAM";
        case MBC5_RUMBLE_RAM_BATTERY:
            return "MBC5+RUMBLE+RAM+BATTERY";
        case MBC6:
            return "MBC6";
        case MBC7_SENSOR_RUMBLE_RAM_BATTERY:
            return "MBC7+SENSOR+RUMBLE+RAM+BATTERY";
        case POCKET_CAMERA:
            return "POCKET CAMERA";
        case BANDAI_TAMA5:
            return "BANDAI TAMA5";
        case HuC3:
            return "HuC3";
        case HuC1_RAM_BATTERY:
            return "HuC1+RAM+BATTERY";
        default:
            return "UNKNOWN";
    }
}