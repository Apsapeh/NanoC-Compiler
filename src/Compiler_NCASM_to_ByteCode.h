#pragma once

#include "Compiler_to_NCASM.h"
#include "VirtualMachine_NC_BYTE-CODE.h"
#include <vector>
#include <map>
#include <string>

class Compiler_NCASM_to_ByteCode {
private:
    std::map <std::string, u_int64_t> regs_bind = {{"tmp_r0", 0}, {"tmp_r1", 1},
                                                         {"math_r0", 2}, {"math_r1", 3},
                                                         {"math_r2", 4}, {"math_r3", 5}};

    std::vector<VirtualMachine_NC_BYTE::ASM_Instruction> compiledNCASM;
    void addInstrToCompiledCode(u_int8_t OPCode, u_int64_t v1_size = 0, int64_t v1_data = 0,
                                                            u_int64_t v2_size = 0, int64_t v2_data = 0);
public:
    Compiler_NCASM_to_ByteCode(std::vector<Compiler_to_NCASM::NCASM_Instruction>* ncasm_code);
};