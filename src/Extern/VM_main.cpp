/*
    Код для компиляции виртуальной машины отдельно от основного компилятора
    /                                                                     /
    Code to compile the virtual machine separately from the main compiler
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

#include "../VirtualMachine_NC_ASM.h"

int main(int argc, char *argv[])
{
    std::string file_name = "";
    if (argc > 1)
    {
        file_name = argv[1];
    }
    else
    {
        std::cout << "Error: Not enough command line arguments\n";
        return -1;
    }

    std::string source_code = "";
    std::string formated_source_code = "";

    std::vector<VirtualMachine_NC_ASM::ASM_Instruction> code;

    std::string *temp_str = new std::string("");
    std::ifstream in(file_name);
    if(in.is_open())
    {
        while(getline(in, *temp_str)) {
            std::string tmp;
            VirtualMachine_NC_ASM::ASM_Instruction tmp_asm;
            int op_num = 0;
            if (*temp_str == "##")
                break;
            for (char &a : *temp_str)
            {
                if (a == ' ' or a == '\n')
                {

                    if (op_num == 0) {
                        tmp_asm.Command = std::stoi(tmp);
                        ++op_num;
                    }
                    else if (op_num == 1) {
                        tmp_asm.val1 = new int64_t(std::stoi(tmp));
                        ++op_num;
                    }
                    else if (op_num == 2) {
                        tmp_asm.val2 = new int64_t(std::stoi(tmp));
                        ++op_num;
                    }

                    tmp.clear();
                }
                else {
                    tmp.push_back(a);
                }
            }

            if (op_num == 1) {
                tmp_asm.val1 = new u_int64_t(std::stoi(tmp));
                ++op_num;
            }
            else if (op_num == 2) {
                tmp_asm.val2 = new u_int64_t(std::stoi(tmp));
                ++op_num;
            }

            code.push_back(tmp_asm);
        }

    }
    else
        std::cout << "Error: File not founded" << std::endl;
    in.close();
    delete temp_str;

    FILE *f = fopen("byte", "w");
    //for (int a = 0; a < 1; ++a)

    for (VirtualMachine_NC_ASM::ASM_Instruction i : code) {
        unsigned char sep = 0;
        fwrite(&i.Command, 1, 1, f);

        if (i.val1 != nullptr) {
            if (*(u_int64_t*)i.val1 < 256)
                fwrite(&*(u_int8_t*)i.val1, 1, 1, f);
            else if (*(u_int64_t*)i.val1 < 65536)
                fwrite(&*(u_int16_t*)i.val1, 2, 1, f);
            else if (*(u_int64_t*)i.val1 < 4294967296)
                fwrite(&*(u_int32_t*)i.val1, 4, 1, f);
            else
                fwrite(&*(u_int64_t*)i.val1, 8, 1, f);
            fwrite(&sep, 1, 1, f);
        }

        if (i.val2 != nullptr) {
            if (*(u_int64_t*)i.val2 < 256)
                fwrite(&*(u_int8_t*)i.val2, 1, 1, f);
            else if (*(u_int64_t*)i.val2 < 65536)
                fwrite(&*(u_int16_t*)i.val2, 2, 1, f);
            else if (*(u_int64_t*)i.val2 < 4294967296)
                fwrite(&*(u_int32_t*)i.val2, 4, 1, f);
            else
                fwrite(&*(u_int64_t*)i.val2, 8, 1, f);
            fwrite(&sep, 1, 1, f);
        }
    }

    fclose(f);


    VirtualMachine_NC_ASM::ASM_Instruction vasm2[code.size()];
    std::copy( code.begin(), code.end(), vasm2 );

    auto start_time = std::chrono::steady_clock::now();
    VirtualMachine_NC_ASM VM(vasm2, sizeof(vasm2)/ sizeof(VirtualMachine_NC_ASM::ASM_Instruction), (unsigned long long) 1, 64);
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    std::cout << elapsed_ns.count() << " ns\n";

    //while (true) {}
}