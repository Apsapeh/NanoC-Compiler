#include <iostream>
#include "Error.h"

void Error::throwError(std::string msg, uint32_t code, std::string file_name, uint32_t line, uint32_t column) {
    std::cerr << msg << "\n";
    exit(code);
}
