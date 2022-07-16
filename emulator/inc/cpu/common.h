#pragma once
#include <cstdint>

union instruction_t {
    uint8_t raw[3];
    struct FIELDS {
        uint8_t operation;
        uint8_t param1;
        uint8_t param2;
    } fields;
};