#include "doctest/doctest.h"
#include "cpu/regs.h"
#include "wrappers/cpu_wrapper.h"
#include "bus.h"
#include "console_logger.h"
#include "mock_bus.h"

#define TEST_CALL \
    cpu.set_regPC(0xA34B); \
    instr.fields.param1 = 0x34; \
    instr.fields.param2 = 0x12; \
    uint16_t old_stack_pointer = cpu.get_regSP(); \
    cpu.exec_explicit_instr(instr); \
    CHECK(cpu.get_regSP() == old_stack_pointer-2); \
    CHECK(mock_bus.read(old_stack_pointer-1) == 0xA3); \
    CHECK(mock_bus.read(old_stack_pointer-2) == 0x4B); \
    CHECK(cpu.get_regPC() == 0x1234);

#define TEST_DONT_CALL \
    cpu.set_regPC(0xCAFE); \
    instr.fields.param1 = 0x34; \
    instr.fields.param2 = 0x12; \
    uint16_t old_stack_pointer = cpu.get_regSP(); \
    cpu.exec_explicit_instr(instr); \
    CHECK(cpu.get_regSP() == old_stack_pointer); \
    CHECK(cpu.get_regPC() == 0xCAFE); \

#define TEST_RST \
    cpu.set_regPC(0xA1B2); \
    uint16_t old_stack_pointer = cpu.get_regSP(); \
    cpu.exec_explicit_instr(instr); \
    CHECK(cpu.get_regSP() == old_stack_pointer-2); \
    CHECK(mock_bus.read(old_stack_pointer-1) == 0xA1); \
    CHECK(mock_bus.read(old_stack_pointer-2) == 0xB2); \

#define TEST_RET \
    /* Push address to the stack */ \
    cpu.set_regSP(0x1000); \
    mock_bus.force_write(0x1000, 0xEF); \
    mock_bus.force_write(0x1001, 0xBE); \
    cpu.exec_explicit_instr(instr); \
    CHECK(cpu.get_regSP() == 0x1002); \
    CHECK(cpu.get_regPC() == 0xBEEF);

#define TEST_DONT_RET \
    cpu.set_regPC(0x100); \
    cpu.set_regSP(0x1000); \
    cpu.exec_explicit_instr(instr); \
    CHECK(cpu.get_regSP() == 0x1000); \
    CHECK(cpu.get_regPC() == 0x100);


TEST_SUITE("JUMP_TESTS") {
    TEST_CASE("JR n") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);

        SUBCASE("JR n - positive") {
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x18);
            mock_bus.force_write(0x101, 0x04);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 + 0x04);
        }

        SUBCASE("JR n - negative") {
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x18);
            mock_bus.force_write(0x101, -0x06);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 - 0x06);
        }
    }

    TEST_CASE("JR NZ,n") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);

        SUBCASE("JR NZ,n - don't jump") {
            cpu.set_flag_Z();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x20);
            mock_bus.force_write(0x101, 0x05);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02);
        }

        SUBCASE("JR NZ,n - jump positive") {
            cpu.clear_flag_Z();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x20);
            mock_bus.force_write(0x101, 0x05);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 + 0x05);
        }

        SUBCASE("JR NZ,n - jump negative") {
            cpu.clear_flag_Z();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x20);
            mock_bus.force_write(0x101, -0x08);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 - 0x08);
        }
    }

    TEST_CASE("JR Z,n") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);

        SUBCASE("JR Z,n - don't jump") {
            cpu.clear_flag_Z();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x28);
            mock_bus.force_write(0x101, 0xFF);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02);
        }

        SUBCASE("JR Z,n - jump positive") {
            cpu.set_flag_Z();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x28);
            mock_bus.force_write(0x101, 0x5F);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 + 0x5F);
        }

        SUBCASE("JR Z,n - jump negative") {
            cpu.set_flag_Z();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x28);
            mock_bus.force_write(0x101, -0x61);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 - 0x61);
        }
    }

    TEST_CASE("JR NC,n") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);

        SUBCASE("JR NC,n - don't jump") {
            cpu.set_flag_C();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x30);
            mock_bus.force_write(0x101, 0xF5);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02);
        }

        SUBCASE("JR NC,n - jump positive") {
            cpu.clear_flag_C();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x30);
            mock_bus.force_write(0x101, 0x15);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 + 0x15);
        }

        SUBCASE("JR NC,n - jump negative") {
            cpu.clear_flag_C();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x30);
            mock_bus.force_write(0x101, -0x41);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 - 0x41);
        }
    }

    TEST_CASE("JR C,n") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);

        SUBCASE("JR C,n - don't jump") {
            cpu.clear_flag_C();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x38);
            mock_bus.force_write(0x101, 0xAA);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02);
        }

        SUBCASE("JR C,n - jump positive") {
            cpu.set_flag_C();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x38);
            mock_bus.force_write(0x101, 0x32);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 + 0x32);
        }

        SUBCASE("JR C,n - jump negative") {
            cpu.set_flag_C();
            cpu.set_regPC(0x100);
            mock_bus.force_write(0x100, 0x38);
            mock_bus.force_write(0x101, -0x01);
            cpu.exec_next_instr();
            CHECK(cpu.get_regPC() == 0x100 + 0x02 - 0x01);
        }
    }

    TEST_CASE("JP") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        instruction_t instr;

        SUBCASE("JP nn") {
            instr.fields.operation = 0xC3;
            instr.fields.param1 = 0x34;
            instr.fields.param2 = 0x12;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regPC() == 0x1234);
        }

        SUBCASE("JP (HL)") {
            cpu.set_regHL(0x1234);
            instr.fields.operation = 0xE9;
            cpu.exec_explicit_instr(instr);
            CHECK(cpu.get_regPC() == 0x1234);
        }
    }

    TEST_CASE("CALL") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        instruction_t instr;

        SUBCASE("CALL nn") {
            instr.fields.operation = 0xCD;
            TEST_CALL
        }

        SUBCASE("CALL NZ,nn - don't call") {
            cpu.set_flag_Z();
            instr.fields.operation = 0xC4;
            TEST_DONT_CALL
        }

        SUBCASE("CALL NZ,nn - call") {
            cpu.clear_flag_Z();
            instr.fields.operation = 0xC4;
            TEST_CALL
        }

        SUBCASE("CALL Z,nn - don't call") {
            cpu.clear_flag_Z();
            instr.fields.operation = 0xCC;
            TEST_DONT_CALL
        }

        SUBCASE("CALL Z,nn - call") {
            cpu.set_flag_Z();
            instr.fields.operation = 0xCC;
            TEST_CALL
        }

        SUBCASE("CALL NC,nn - don't call") {
            cpu.set_flag_C();
            instr.fields.operation = 0xD4;
            TEST_DONT_CALL
        }

        SUBCASE("CALL NC,nn - call") {
            cpu.clear_flag_C();
            instr.fields.operation = 0xD4;
            TEST_CALL
        }

        SUBCASE("CALL C,nn - don't call") {
            cpu.clear_flag_C();
            instr.fields.operation = 0xDC;
            TEST_DONT_CALL
        }

        SUBCASE("CALL C,nn - call") {
            cpu.set_flag_C();
            instr.fields.operation = 0xDC;
            TEST_CALL
        }
    }

    TEST_CASE("RST") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        instruction_t instr;

        SUBCASE("RST 00H") {
            instr.fields.operation = 0xC7;
            TEST_RST
            CHECK(cpu.get_regPC() == 0x0000);
        }

        SUBCASE("RST 08H") {
            instr.fields.operation = 0xCF;
            TEST_RST
            CHECK(cpu.get_regPC() == 0x0008);
        }

        SUBCASE("RST 10H") {
            instr.fields.operation = 0xD7;
            TEST_RST
            CHECK(cpu.get_regPC() == 0x0010);
        }

        SUBCASE("RST 18H") {
            instr.fields.operation = 0xDF;
            TEST_RST
            CHECK(cpu.get_regPC() == 0x0018);
        }

        SUBCASE("RST 20H") {
            instr.fields.operation = 0xE7;
            TEST_RST
            CHECK(cpu.get_regPC() == 0x0020);
        }

        SUBCASE("RST 28H") {
            instr.fields.operation = 0xEF;
            TEST_RST
            CHECK(cpu.get_regPC() == 0x0028);
        }

        SUBCASE("RST 30H") {
            instr.fields.operation = 0xF7;
            TEST_RST
            CHECK(cpu.get_regPC() == 0x0030);
        }

        SUBCASE("RST 38H") {
            instr.fields.operation = 0xFF;
            TEST_RST
            CHECK(cpu.get_regPC() == 0x0038);
        }
    }

    TEST_CASE("RET") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        instruction_t instr;

        SUBCASE("RET") {
            instr.fields.operation = 0xC9;
            TEST_RET
        }

        SUBCASE("RET NZ - don't return") {
            cpu.set_flag_Z();
            instr.fields.operation = 0xC0;
            TEST_DONT_RET
        }

        SUBCASE("RET NZ - return") {
            cpu.clear_flag_Z();
            instr.fields.operation = 0xC0;
            TEST_RET
        }

        SUBCASE("RET Z - don't return") {
            cpu.clear_flag_Z();
            instr.fields.operation = 0xC8;
            TEST_DONT_RET
        }

        SUBCASE("RET Z - return") {
            cpu.set_flag_Z();
            instr.fields.operation = 0xC8;
            TEST_RET
        }

        SUBCASE("RET NC - don't return") {
            cpu.set_flag_C();
            instr.fields.operation = 0xD0;
            TEST_DONT_RET
        }

        SUBCASE("RET NC - return") {
            cpu.clear_flag_C();
            instr.fields.operation = 0xD0;
            TEST_RET
        }

        SUBCASE("RET C - don't return") {
            cpu.clear_flag_C();
            instr.fields.operation = 0xD8;
            TEST_DONT_RET
        }

        SUBCASE("RET C - return") {
            cpu.set_flag_C();
            instr.fields.operation = 0xD8;
            TEST_RET
        }
    }

    TEST_CASE("RETI") {
        MockBus mock_bus;
        ConsoleLogger logger;
        CPUWrapper cpu (mock_bus, logger);
        instruction_t instr;

        mock_bus.io.interrupts.disable_IME_flag();
        CHECK_FALSE(mock_bus.io.interrupts.debug_get_IME_flag());
        instr.fields.operation = 0xD9;
        TEST_RET
        CHECK(mock_bus.io.interrupts.debug_get_IME_flag());
    }
}
