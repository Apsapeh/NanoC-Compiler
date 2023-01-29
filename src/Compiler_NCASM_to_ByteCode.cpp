#include "Compiler_NCASM_to_ByteCode.h"

Compiler_NCASM_to_ByteCode::Compiler_NCASM_to_ByteCode(std::vector<Compiler_to_NCASM::NCASM_Instruction>* ncasm_code) {
    std::map<std::string, u_int64_t> vars_heap_binds;
    u_int64_t heap_size = 0;

    for (Compiler_to_NCASM::NCASM_Instruction instr : *ncasm_code)
    {
        std::string OPCode = instr.OPcode;

        if (OPCode == "PUSH_TO_STACK") {
            addInstrToCompiledCode(5, 8);
            vars_heap_binds[instr.Args[1]] = heap_size;
            ++heap_size;
        }
        else if (OPCode == "LOAD") {
            auto iter = vars_heap_binds.find(instr.Args[0]);
            if (iter == vars_heap_binds.end()) {
                std::cout << "Error: var \"" << instr.Args[0] << "is not defined";
                exit(1);
            }
            addInstrToCompiledCode(6, 8, iter->second, 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "SET") {
            addInstrToCompiledCode(10, 8, std::stoll(instr.Args[0]), 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "MOV") {
            addInstrToCompiledCode(9, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }

        else if (OPCode == "ADD") {
            addInstrToCompiledCode(11, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "MINUS") {
            addInstrToCompiledCode(12, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "MULTIPLE") {
            addInstrToCompiledCode(13, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "DIVIDE") {
            addInstrToCompiledCode(14, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "JEQ") {

        }
        else if (OPCode == "JNEQZ") {

        }
        else if (OPCode == "JLESS") {

        }
        else if (OPCode == "JMORE") {

        }
        else if (OPCode == "JNEQ") {

        }
        else if (OPCode == "JNNEQ") {

        }
        else if (OPCode == "JNLESS") {

        }
        else if (OPCode == "JNMORE") {

        }
        else if (OPCode == "JMP") {

        }
        else if (OPCode == "CONVERT_TO_LOGICAL") {
            addInstrToCompiledCode(24, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "LAND") {
            addInstrToCompiledCode(25, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "LOR") {
            addInstrToCompiledCode(26, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "LNOT") {
            addInstrToCompiledCode(27, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }

    }
}

void Compiler_NCASM_to_ByteCode::addInstrToCompiledCode(u_int8_t OPCode, u_int64_t v1_size, int64_t v1_data,
                                                        u_int64_t v2_size, int64_t v2_data) {
    VirtualMachine_NC_BYTE::ASM_Instruction temp;
    temp.Command = OPCode;
    if (v1_size != 0)
    {
        temp.val1 = malloc(v1_size);
        *(int64_t*)temp.val1 = v1_data;
    }
    if (v2_size != 0)
    {
        temp.val2 = malloc(v2_size);
        *(int64_t*)temp.val2 = v2_data;
    }
}

int64_t getJMP_Size(std::vector<Compiler_to_NCASM::NCASM_Instruction>* ncasm_code,
                    u_int64_t start_pose, std::string el_to_find)
{
    u_int64_t count_of_bad = 1;

    for (u_int64_t index=start_pose; index < ncasm_code->size(), ++index;)
    {
        if (el_to_find == "ELSE_BEGIN" or el_to_find == "ELSE_END") {
            if ((*ncasm_code)[index].OPcode == "IF_BEGIN")
            {
                ++count_of_bad;
            }
            else if ((*ncasm_code)[index].OPcode == "ELSE_BEGIN")
            {
                --count_of_bad;
            }

            if ((*ncasm_code)[index].OPcode == el_to_find and count_of_bad == 0)
            //(*ncasm_code)[index].OPcode == "IF_BEGIN"
        }
    }
}
