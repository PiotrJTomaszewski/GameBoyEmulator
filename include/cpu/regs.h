#pragma once
#include <cstdint>

union reg_16bit_t {
    uint16_t value;
    struct REG16BIT_BYTE_PAIR {
        uint8_t lower;
        uint8_t higher;
    } pair;
};

union flags_reg_t {
    uint8_t value;
    struct FLAGS_REG_INNER {
        unsigned Z: 1; // zero
        unsigned N: 1; // substract
        unsigned H: 1; // half carry
        unsigned C: 1; // carry
        unsigned _unused: 4;
    } flags;
};
