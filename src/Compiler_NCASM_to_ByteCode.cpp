#include "Compiler_NCASM_to_ByteCode.h"
#include <fstream>
#include <stdlib.h>

int64_t getJMP_Size(std::vector<Compiler_to_NCASM::NCASM_Instruction>* ncasm_code,
                    u_int64_t start_pose, std::string el_to_find);

Compiler_NCASM_to_ByteCode::Compiler_NCASM_to_ByteCode(std::vector<Compiler_to_NCASM::NCASM_Instruction>* ncasm_code) {
    std::map<std::string, u_int64_t> vars_heap_binds;
    u_int64_t heap_size = 0;

    u_int64_t num = 0;
    for (Compiler_to_NCASM::NCASM_Instruction instr : *ncasm_code)
    {
        std::string OPCode = instr.OPcode;

        if (OPCode == "PUSH_TO_STACK") {
            addInstrToCompiledCode(5, 8, 8);
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

        else if (OPCode == "COPY") {
            addInstrToCompiledCode(8, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "MOV") {
            addInstrToCompiledCode(9, 8, regs_bind[instr.Args[0]], 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "SET") {
            addInstrToCompiledCode(10, 8, std::stoll(instr.Args[0]), 8, regs_bind[instr.Args[1]]);
        }
        else if (OPCode == "SET_NEW") {
            addInstrToCompiledCode(51, 8, std::stoll(instr.Args[0]), 8, regs_bind[instr.Args[1]]);
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
            addInstrToCompiledCode(15, 8, getJMP_Size(ncasm_code, num, instr.Args[1]), 8, regs_bind[instr.Args[0]]);
        }
        else if (OPCode == "JNEQZ") {
            addInstrToCompiledCode(16, 8, getJMP_Size(ncasm_code, num, instr.Args[1]), 8, regs_bind[instr.Args[0]]);
        }
        else if (OPCode == "JLESS") {
            addInstrToCompiledCode(17, 8, getJMP_Size(ncasm_code, num, instr.Args[1]), 8, regs_bind[instr.Args[0]]);
        }
        else if (OPCode == "JMORE") {
            addInstrToCompiledCode(18, 8, getJMP_Size(ncasm_code, num, instr.Args[1]), 8, regs_bind[instr.Args[0]]);
        }
        else if (OPCode == "JNEQ") {
            addInstrToCompiledCode(19, 8, getJMP_Size(ncasm_code, num, instr.Args[1]), 8, regs_bind[instr.Args[0]]);
        }
        else if (OPCode == "JNNEQ") {
            addInstrToCompiledCode(20, 8, getJMP_Size(ncasm_code, num, instr.Args[1]), 8, regs_bind[instr.Args[0]]);
        }
        else if (OPCode == "JNLESS") {
            addInstrToCompiledCode(21, 8, getJMP_Size(ncasm_code, num, instr.Args[1]), 8, regs_bind[instr.Args[0]]);
        }
        else if (OPCode == "JNMORE") {
            addInstrToCompiledCode(22, 8, getJMP_Size(ncasm_code, num, instr.Args[1]), 8, regs_bind[instr.Args[0]]);
        }
        else if (OPCode == "JMP") {
            addInstrToCompiledCode(23, 8, getJMP_Size(ncasm_code, num, instr.Args[0]));
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

        else if (OPCode == "PRINT") {
            addInstrToCompiledCode(50, 8, regs_bind[instr.Args[0]]);
        }

        ++num;
    }

    u_int64_t num5 = 0;
    for (VirtualMachine_NC_BYTE::ASM_Instruction instr : compiledNCASM) {
        std::cout << num5 << " --- " << (int)instr.Command;
        if (instr.val1 != nullptr)
            std::cout << " " << *(int64_t*)instr.val1;
        if (instr.val2 != nullptr)
            std::cout << " " << *(int64_t*)instr.val2;
        std::cout << "\n";
        ++num5;
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

    compiledNCASM.push_back(temp);
}

std::vector<VirtualMachine_NC_BYTE::ASM_Instruction> Compiler_NCASM_to_ByteCode::getCompiledCode() {
    return compiledNCASM;
}

void Compiler_NCASM_to_ByteCode::saveCompiledProgram_to_file(std::string FileName) {
    FILE *f = fopen("byte", "wb");

    unsigned char tmp;
    for (VirtualMachine_NC_BYTE::ASM_Instruction instr : compiledNCASM) {
        std::cout << (int)instr.Command << std::endl;
        fwrite(&instr.Command, 1, 1, f);

        if (instr.val1 != nullptr)
        {
            if (*(int64_t*)instr.val1 > -129 and *(int64_t*)instr.val1 < 128){
                tmp = 1;
                fwrite(&tmp, 1, 1, f);
                fwrite((int8_t*)instr.val1, 1, 1, f);
            }
            else if (*(int64_t*)instr.val1 > -32769 and *(int64_t*)instr.val1 < 32768) {
                tmp = 2;
                fwrite(&tmp, 1, 1, f);
                fwrite((int16_t*)instr.val1, 2, 1, f);
            }
            else if (*(int64_t*)instr.val1 > -2147483649 and *(int64_t*)instr.val1 < 2147483648) {
                tmp = 4;
                fwrite(&tmp, 1, 1, f);
                fwrite((int32_t*)instr.val1, 4, 1, f);
            }
            else {
                tmp = 8;
                fwrite(&tmp, 1, 1, f);
                fwrite((int64_t*)instr.val1, 8, 1, f);
            }
        }

        if (instr.val2 != nullptr)
        {
            if (*(int64_t*)instr.val2 > -129 and *(int64_t*)instr.val2 < 128){
                tmp = 1;
                fwrite(&tmp, 1, 1, f);
                fwrite((int8_t*)instr.val2, 1, 1, f);
            }
            else if (*(int64_t*)instr.val2 > -32769 and *(int64_t*)instr.val2 < 32768) {
                tmp = 2;
                fwrite(&tmp, 1, 1, f);
                fwrite((int16_t*)instr.val2, 2, 1, f);
            }
            else if (*(int64_t*)instr.val2 > -2147483649 and *(int64_t*)instr.val2 < 2147483648) {
                tmp = 4;
                fwrite(&tmp, 1, 1, f);
                fwrite((int32_t*)instr.val2, 4, 1, f);
            }
            else {
                tmp = 8;
                fwrite(&tmp, 1, 1, f);
                fwrite((int64_t*)instr.val2, 8, 1, f);
            }
        }
        /*if (instr.val2 != nullptr)
        {
            tmp = 8;
            fwrite(&tmp, 1, 1, f);
            fwrite((int64_t*)instr.val2, 8, 1, f);
        }*/
    }
    tmp = 0;
    fwrite(&tmp, 1, 1, f);
    fclose(f);

    /*std::string str;
    for (VirtualMachine_NC_BYTE::ASM_Instruction instr : compiledNCASM) {
        str += std::to_string((int)instr.Command);
        if (instr.val1 != nullptr)
            str += " " + std::to_string(*(int64_t*)instr.val1);
        if (instr.val2 != nullptr)
            str += " " + std::to_string(*(int64_t*)instr.val2);
        str += "\n";
    }

    std::ofstream out;          // поток для записи
    out.open(FileName); // окрываем файл для записи
    if (out.is_open())
    {
        out << str;
    }
    out.close();*/


}

int64_t getJMP_Size(std::vector<Compiler_to_NCASM::NCASM_Instruction>* ncasm_code,
                    u_int64_t start_pose, std::string el_to_find)
{
    u_int64_t count_of_bad = 1;
    u_int64_t count_of_labels = 0;

    if (el_to_find == "ELSE_BEGIN" or el_to_find == "ELSE_END" or el_to_find == "WHILE_END") {
        for (u_int64_t index = start_pose; index < ncasm_code->size(), ++index;) {
            if ((*ncasm_code)[index].OPcode == "IF_BEGIN" or (*ncasm_code)[index].OPcode == "ELSE_BEGIN" or
                (*ncasm_code)[index].OPcode == "ELSE_END" or (*ncasm_code)[index].OPcode == "WHILE_BEGIN" or
                (*ncasm_code)[index].OPcode == "WHILE_END")
            {
                ++count_of_labels;
            }

            if (((*ncasm_code)[index].OPcode == "IF_BEGIN" and el_to_find != "WHILE_END") or
                ((*ncasm_code)[index].OPcode == "WHILE_BEGIN" and el_to_find == "WHILE_END"))
            {
                ++count_of_bad;
            }
            else if (((*ncasm_code)[index].OPcode == "ELSE_BEGIN" and el_to_find != "WHILE_END") or
                     ((*ncasm_code)[index].OPcode == "WHILE_END" and el_to_find == "WHILE_END")) {
                --count_of_bad;
            }

            if ((*ncasm_code)[index].OPcode == el_to_find and count_of_bad == 0) {
                return index - start_pose - count_of_labels;
            }
            //(*ncasm_code)[index].OPcode == "IF_BEGIN"

        }
    }
    else if (el_to_find == "WHILE_BEGIN") {
        for (u_int64_t index = start_pose; index > 0, --index;) {
            if ((*ncasm_code)[index].OPcode == "IF_BEGIN" or (*ncasm_code)[index].OPcode == "ELSE_BEGIN" or
                (*ncasm_code)[index].OPcode == "ELSE_END" or (*ncasm_code)[index].OPcode == "WHILE_BEGIN" or
                (*ncasm_code)[index].OPcode == "WHILE_END")
            {
                ++count_of_labels;
            }

            if ((*ncasm_code)[index].OPcode == "WHILE_END") {
                ++count_of_bad;
            }
            else if ((*ncasm_code)[index].OPcode == "WHILE_BEGIN") {
                --count_of_bad;
            }

            if ((*ncasm_code)[index].OPcode == el_to_find and count_of_bad == 0) {
                return index - start_pose + count_of_labels - 1;
            }
            //(*ncasm_code)[index].OPcode == "IF_BEGIN"

        }
    }
}
