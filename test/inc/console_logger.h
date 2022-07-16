#pragma once
#include "logger.h"

class ConsoleLogger: public Logger {
public:
    void log(std::string message);
};
