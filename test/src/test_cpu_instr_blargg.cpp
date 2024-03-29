// Test the CPU using Blargg's CPU instructions tests
#include <string>
#include <cstring>
#include <iostream>
#include "doctest/doctest.h"
#include "wrappers/bus_wrapper.h"
#include "wrappers/cpu_wrapper.h"
#include "console_logger.h"

const std::string BLARGG_CPU_TESTS_DIR = "../../test/test_roms/gb-test-roms/cpu_instrs/individual/";

#define BLARGG_CPU_TEST(file_name, expected_new_line_count) \
    bool test_running = true; \
    unsigned long long total_cycles = 0; \
    unsigned cycles = 0; \
    BusWrapper bus(true); \
    ConsoleLogger logger; \
    CPUWrapper cpu(bus, logger); \
    bus.load_cartridge_from_file(BLARGG_CPU_TESTS_DIR + file_name); \
    bool timeout_occured = false; \
    const char *passed_str_pos; \
    while (test_running) { \
        cycles = cpu.exec_next_instr(); \
        bus.io.timer.tick(cycles); \
        total_cycles += cycles; \
        if (bus.get_serial_data_newline_count() >= expected_new_line_count) { \
            test_running = false; \
        } \
        if (total_cycles > 1e9) { \
            std::cout << "Timeout" << std::endl; \
            timeout_occured = true; \
            test_running = false; \
        } \
    } \
    passed_str_pos = strstr(bus.get_serial_data_log(), "Passed"); \
    if (passed_str_pos == nullptr) { \
        std::cout << "-------- " << file_name << " --------" << std::endl; \
        std::cout << bus.get_serial_data_log() << std::endl << std::endl << std::endl; \
    } \
    CHECK_FALSE(timeout_occured); \
    CHECK(passed_str_pos != nullptr);


TEST_SUITE("Blargg CPU Instrucions Tests") {
    TEST_CASE("01-special.gb") {
        BLARGG_CPU_TEST("01-special.gb", 4);
    }
    TEST_CASE("02-interrupts.gb") {
        BLARGG_CPU_TEST("02-interrupts.gb", 4);
    }
    TEST_CASE("03-op sp,hl.gb") {
        BLARGG_CPU_TEST("03-op sp,hl.gb", 4);
    }
    TEST_CASE("04-op r,imm.gb") {
        BLARGG_CPU_TEST("04-op r,imm.gb", 4);
    }
    TEST_CASE("05-op rp.gb") {
        BLARGG_CPU_TEST("05-op rp.gb", 4);
    }
    TEST_CASE("06-ld r,r.gb") {
        BLARGG_CPU_TEST("06-ld r,r.gb", 4);
    }
    TEST_CASE("07-jr,jp,call,ret,rst.gb") {
        BLARGG_CPU_TEST("07-jr,jp,call,ret,rst.gb", 4);
    }
    TEST_CASE("08-misc instrs.gb") {
        BLARGG_CPU_TEST("08-misc instrs.gb", 4);
    }
    TEST_CASE("09-op r,r.gb") {
        BLARGG_CPU_TEST("09-op r,r.gb", 4);
    }
    TEST_CASE("10-bit ops.gb") {
        BLARGG_CPU_TEST("10-bit ops.gb", 4);
    }
    TEST_CASE("11-op a,(hl).gb") {
        BLARGG_CPU_TEST("11-op a,(hl).gb", 4);
    }
}
