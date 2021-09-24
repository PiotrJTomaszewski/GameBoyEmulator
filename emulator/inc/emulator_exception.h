#pragma once
#include <exception>
#include <cstdarg>

class EmulatorException: public std::exception {
public:
    EmulatorException(const char *format, ...) {
        va_list valist;
        va_start(valist, format);
        vsprintf(buffer, format, valist);
        va_end(valist);
    }

    const char *what() const throw() { 
        return buffer; 
    }

private:
    char buffer[50];
};
