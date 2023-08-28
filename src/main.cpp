#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "main.h"
#include "PreProcessor.h"
#include "Lexer.h"
#include "Parser.h"
#include "Compiler_to_NCASM.h"
#include "Compiler_NCASM_to_ByteCode.h"
#include "VirtualMachine_NC_BYTE-CODE.h"

int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
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
        {
            source_code += *temp_str + "\n";
           // std::cout << *temp_str << std::endl;
        }
    }
    else
        std::cout << "Error: File not founded" << std::endl;
    in.close();

    *temp_str = "";
    formated_source_code = source_code;
    for (unsigned long long ch_num=0; ch_num < formated_source_code.length(); ++ch_num)
    {
        if (formated_source_code[ch_num] == '\t')
        {
            formated_source_code.replace(ch_num, 1, " ");
            --ch_num;
        }
    }
    std::map <std::string, int> number_of_brackets = {{"round_st",  0}, {"round_end",  0}, // Кол-во круглых скобок
                                                      {"figure_st", 0}, {"figure_end", 0}};// Кол-во фигурных скобок
    for (char ch : source_code)
    {
        if (ch == '{')
            ++number_of_brackets["figure_st"];
        else if (ch == '}')
            ++number_of_brackets["figure_end"];
        else if (ch == '(')
            ++number_of_brackets["round_st"];
        else if (ch == ')')
            ++number_of_brackets["round_end"];
    }

    if (number_of_brackets["figure_st"] != number_of_brackets["figure_end"])
    {
        int difference = number_of_brackets["figure_st"] - number_of_brackets["figure_end"];
        char miss_char = difference > 0  ?  '}'  :  '{';
        
        for (uint32_t num = 0; num < abs(difference); ++num)
            std::cout << "Syntax error: character `" << miss_char << "` not found" << std::endl;
        
        return -1;
    }

    if (number_of_brackets["round_st"] != number_of_brackets["round_end"])
    {
        int difference = number_of_brackets["round_st"] - number_of_brackets["round_end"];
        char miss_char = difference > 0  ?  ')'  :  '(';
        
        for (uint32_t num = 0; num < abs(difference); ++num)
            std::cout << "Syntax error: character `" << miss_char << "` not found" << std::endl;
        
        return -1;
    }

    PreProcessor preProcessor(source_code);
    Lexer lexer(preProcessor.getProcessedSourceVecLines());
    Parser parser(lexer.getLexedSource());
    //Compiler_to_NCASM comp_to_ncasm(parser.getParsedProgram());
    //Compiler_NCASM_to_ByteCode comp_ncasm_to_byte(comp_to_ncasm.getCompiledCode());
    //comp_ncasm_to_byte.saveCompiledProgram_to_file("asm.asm");

    //std::vector<VirtualMachine_NC_BYTE::ASM_Instruction> vasm = comp_ncasm_to_byte.getCompiledCode();

    //VirtualMachine_NC_BYTE VM(vasm.data(), vasm.size() * sizeof(VirtualMachine_NC_BYTE::ASM_Instruction), (unsigned long long) 1, 64);

    delete temp_str;
}


