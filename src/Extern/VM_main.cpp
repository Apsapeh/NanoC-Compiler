/*
    Код для компиляции виртуальной машины отдельно от основного компилятора
    /                                                                     /
    Code to compile the virtual machine separately from the main compiler
*/

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "../VirtualMachine_NC_ASM.h"

int main(int argc, char *argv[])
{
    /*std::string file_name = "";
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

    std::string *temp_str = new std::string("");
    std::ifstream in(file_name);
    if(in.is_open())
    {
        while(getline(in, *temp_str))
            source_code += *temp_str + "\n";
    }
    else
        std::cout << "Error: File not founded" << std::endl;
    in.close();

    delete temp_str;*/

//    VirtualMachine_NC_ASM::ASM_Instruction vasm[1] = {{1, calloc(1, 1), calloc(1, 4)}};
    VirtualMachine_NC_ASM::ASM_Instruction vasm[]  {{1, malloc(8), malloc(8)},
                                                    {6, malloc(8)},
                                                    {5, malloc(8), malloc(8)},
                                                    {6, malloc(8)},
                                                    {2, malloc(8)}};
    *(u_int64_t*)vasm[0].val1 = 0;
    *(u_int64_t*)vasm[0].val2 = 4;
    *(u_int64_t*)vasm[1].val1 = 0;
    *(u_int64_t*)vasm[2].val1 = 0;
    *(u_int64_t*)vasm[2].val2 = 4;
    *(u_int64_t*)vasm[3].val1 = 0;
    *(u_int64_t*)vasm[4].val1 = 0;
    std::cout << vasm[0].val1 << std::endl << vasm[0].val2 << std::endl;
    VirtualMachine_NC_ASM VM(vasm, (unsigned long long) 5, (unsigned long long) 1);

    //while (true) {}
}