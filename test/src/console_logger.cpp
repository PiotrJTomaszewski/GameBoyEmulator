#include "console_logger.h"
#include <iostream>

void ConsoleLogger::log(std::string message) {
    std::cout << message << std::endl;
}

void ConsoleLogger::log_instruction(instruction_t const& instruction) {
    std::cout << std::hex << instruction.fields.operation << " " << instruction.fields.param1 << " " << instruction.fields.param2 << std::endl;
}
