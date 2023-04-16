#pragma once

#include <string>

namespace Error {
    void throwError(std::string msg, u_int32_t code = 1, std::string file_name = "", u_int32_t line = 0, u_int32_t column = 0);
    void throwWarning();
};