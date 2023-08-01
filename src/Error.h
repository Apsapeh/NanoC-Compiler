#pragma once

#include <string>
#include <cstdint>

namespace Error {
    void throwError(std::string msg, uint32_t code = 1, std::string file_name = "", uint32_t line = 0, uint32_t column = 0);
    void throwWarning();
};