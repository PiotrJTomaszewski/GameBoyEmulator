#pragma once
#include <cstdint>

union __attribute__((packed)) reg_16bit_t {
    uint16_t value;
    struct __attribute__((packed)) REG16BIT_BYTE_PAIR {
        uint8_t lower;
        uint8_t higher;
    } pair;
};

union __attribute__((packed)) flags_reg_t {
    uint8_t value;
    struct __attribute__((packed)) FLAGS_REG_INNER {
        unsigned _unused: 4;
        unsigned C: 1; // carry
        unsigned H: 1; // half carry
        unsigned N: 1; // substract
        unsigned Z: 1; // zero
    } flags;
};
