/*
    Код для компиляции виртуальной машины отдельно от основного компилятора
    /                                                                     /
    Code to compile the virtual machine separately from the main compiler
*/

#include <iostream>
#include <vector>
#include <chrono>

#include "../VirtualMachine_NC_BYTE-CODE.h"

typedef uint64_t u_int64_t;
typedef uint32_t u_int32_t;
typedef uint16_t u_int16_t;
typedef uint8_t u_int8_t;

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);

    std::string file_name = "";
    if (argc > 1) {
        file_name = argv[1];
    } else {
        std::cout << "Error: Not enough command line arguments\n";
        return -1;
    }

    std::vector<VirtualMachine_NC_BYTE::ASM_Instruction> code;

    FILE *f = fopen("byte", "rb");

    u_int8_t arg_count = 0;
    u_int8_t arg_num = 0;
    u_int8_t op = 0;
    u_int8_t current_length = 0;
    VirtualMachine_NC_BYTE::ASM_Instruction temp_asm_instr;
    while (true) {
        if (arg_count == 0) {
            if (op != 0) {
                code.push_back(temp_asm_instr);
                temp_asm_instr.val1 = nullptr;
                temp_asm_instr.val2 = nullptr;
            }

            fread(&op, 1, 1, f);
            if (op == 0){
                break;
            }
            temp_asm_instr.Command = op;
            arg_num = 0;

            if (op == 3 or op == 5 or op == 7 or op == 23 or op == 50)
                arg_count = 1;
            else
                arg_count = 2;
        }
        else {
            if (current_length == 0) {
                fread(&current_length, 1, 1, f);
            }
            else {
                int8_t arg;
                fread(&arg, current_length, 1, f);
                if (arg_num == 0)
                    temp_asm_instr.val1 = new int64_t(arg);
                else
                    temp_asm_instr.val2 = new int64_t(arg);

                current_length = 0;
                ++arg_num;
                --arg_count;
            }
        }
    }


    VirtualMachine_NC_BYTE::ASM_Instruction vasm2[code.size()];
    std::copy( code.begin(), code.end(), vasm2 );

    auto start_time = std::chrono::steady_clock::now();
    VirtualMachine_NC_BYTE VM(vasm2, sizeof(vasm2)/ sizeof(VirtualMachine_NC_BYTE::ASM_Instruction), (unsigned long long) 1, 64);
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    std::cout << elapsed_ns.count() << " ns\n";

    //while (true) {}
}