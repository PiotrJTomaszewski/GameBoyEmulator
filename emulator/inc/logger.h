#pragma once
#include <string>
#include "cpu/common.h"

class Logger {
public:
    virtual void log(std::string msg) = 0;
    virtual void log_instruction(instruction_t const& instruction) = 0;
};
