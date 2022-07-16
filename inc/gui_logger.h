#pragma once
#include "logger.h"
#include <vector>

class GuiLogger: public Logger {
public:
    void log(std::string message);
    void display() const;

private:
    std::vector<std::string> messages;
};
