#include "console_logger.h"
#include <iostream>

void ConsoleLogger::log(std::string message) {
    std::cout << message << std::endl;
}
