#include "Error.h"

void Error::throwError(std::string msg, u_int32_t code, std::string file_name, u_int32_t line, u_int32_t column) {
    exit(code);
}
