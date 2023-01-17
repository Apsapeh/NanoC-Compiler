/*
    Код для компиляции виртуальной машины отдельно от основного компилятора
    /                                                                     /
    Code to compile the virtual machine separately from the main compiler
*/

#include <iostream>
#include <fstream>

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

    delete temp_str;

    std::cout << source_code << std::endl;
}