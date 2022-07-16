#include "doctest/doctest.h"
#include "cpu/regs.h"
#include "wrappers/cpu_wrapper.h"
#include "bus.h"
#include "console_logger.h"
#include "mock_bus.h"


TEST_SUITE("ALU Tests") {
    TEST_CASE("ADD") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);

        SUBCASE("0xAF + 0x74; A+B") {
            cpu.set_regA(0xAF);
            cpu.set_regB(0x74);
            // ADD A,B
            mock_bus.force_write(cpu.get_regPC(), 0x80);
            cpu.exec_next_instr();
            CHECK(cpu.get_regA() == 0x23);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 1);
            CHECK(cpu.get_flag_C() == 1);
        }

        SUBCASE("0xBA + 0x35; A+C") {
            cpu.set_regA(0xBA);
            cpu.set_regC(0x35);
            // ADD A,C
            mock_bus.force_write(cpu.get_regPC(), 0x81);
            cpu.exec_next_instr();
            CHECK(cpu.get_regA() == 0xEF);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 0);
            CHECK(cpu.get_flag_C() == 0);
        }

        SUBCASE("0xF1 + 0x1F; A+D") {
            cpu.set_regA(0xF1);
            cpu.set_regD(0x1F);
            // ADD A,D
            mock_bus.force_write(cpu.get_regPC(), 0x82);
            cpu.exec_next_instr();
            CHECK(cpu.get_regA() == 0x10);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 1);
            CHECK(cpu.get_flag_C() == 1);
        }

        SUBCASE("0x08 + 0x08 - H flag; A+A") {
            cpu.set_regA(0x08);
            // ADD A,A
            mock_bus.force_write(cpu.get_regPC(), 0x87);
            cpu.exec_next_instr();
            CHECK(cpu.get_regA() == 0x10);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 1);
            CHECK(cpu.get_flag_C() == 0);
        }

        SUBCASE("0x80 + 0x80 - Z flag; A+(HL)") {
            cpu.set_regA(0x80);
            // ADD A,(HL)
            mock_bus.force_write(cpu.get_regPC(), 0x87);
            cpu.set_regHL(0x0010);
            mock_bus.force_write(0x0010, 0x80);
            cpu.exec_next_instr();
            CHECK(cpu.get_regA() == 0x00);
            CHECK(cpu.get_flag_Z() == 1);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 0);
            CHECK(cpu.get_flag_C() == 1);
        }
    }

    TEST_CASE("AND") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        CPUWrapper::instruction_t instr;

        SUBCASE("0x15 & 0x53; A&B") {
            cpu.set_regA(0x15);
            cpu.set_regB(0x53);
            // AND B
            instr.fields.operation = 0xA0;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0x11);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 1);
            CHECK(cpu.get_flag_C() == 0);
        }

        SUBCASE("0x59 & 0xA6 - Z flag; A&(HL)") {
            cpu.set_regA(0x59);            
            // AND (HL)
            instr.fields.operation = 0xA6;
            cpu.set_regHL(0x0010);
            mock_bus.force_write(0x0010, 0xA6);
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0x00);
            CHECK(cpu.get_flag_Z() == 1);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 1);
            CHECK(cpu.get_flag_C() == 0);
        }
    }

    TEST_CASE("SUB") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        CPUWrapper::instruction_t instr;

        SUBCASE("0xD6 - 0xDE") {
            cpu.set_regA(0xD6);
            cpu.set_regB(0xDE);
            // SUB B
            instr.fields.operation = 0x90;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0xF8);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 1);
            CHECK(cpu.get_flag_H() == 1);
            CHECK(cpu.get_flag_C() == 1);
        }
    }

    TEST_CASE("RLCA") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        CPUWrapper::instruction_t instr;

        SUBCASE("0x80") {
            cpu.clear_flag_C();
            cpu.set_regA(0x80);
            // RLCA
            instr.fields.operation = 0x07;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0x01);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 0);
            CHECK(cpu.get_flag_C() == 1);
        }

        SUBCASE("0x17") {
            cpu.set_flag_C();
            cpu.set_regA(0x17);
            // RLCA
            instr.fields.operation = 0x07;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0x2E);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 0);
            CHECK(cpu.get_flag_C() == 0);
        }
    }

    TEST_CASE("RLA") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        CPUWrapper::instruction_t instr;
        SUBCASE("0x70 - C=1") {
            cpu.set_flag_C();
            cpu.set_regA(0x70);
            // RLA
            instr.fields.operation = 0x17;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0xE1);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 0);
            CHECK(cpu.get_flag_C() == 0);
        }

        SUBCASE("0x80 - C=0") {
            cpu.clear_flag_C();
            cpu.set_regA(0x80);
            // RLA
            instr.fields.operation = 0x17;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0x00);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 0);
            CHECK(cpu.get_flag_C() == 1);
        }
    }

    TEST_CASE("RRCA") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        CPUWrapper::instruction_t instr;
        SUBCASE("0x01") {
            cpu.clear_flag_C();
            cpu.set_regA(0x01);
            // RRCA
            instr.fields.operation = 0x0F;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0x80);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 0);
            CHECK(cpu.get_flag_C() == 1);
        }
    }

    TEST_CASE("RRA") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        CPUWrapper::instruction_t instr;
        SUBCASE("0x80 - C=1") {
            cpu.set_flag_C();
            cpu.set_regA(0x80);
            // RRCA
            instr.fields.operation = 0x1F;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0xC0);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 0);
            CHECK(cpu.get_flag_C() == 0);
        }

        SUBCASE("0x01 - C=0") {
            cpu.clear_flag_C();
            cpu.set_regA(0x01);
            // RRCA
            instr.fields.operation = 0x1F;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regA() == 0x00);
            CHECK(cpu.get_flag_Z() == 0);
            CHECK(cpu.get_flag_N() == 0);
            CHECK(cpu.get_flag_H() == 0);
            CHECK(cpu.get_flag_C() == 1);
        }
    }
}
