#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "main.h"
#include "Lexer.h"
#include "Parser.h"
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
    //formated_source_code += *temp_str;

   // std::cout << source_code << std::endl;
    //std::cout << formated_source_code << std::endl;
    //std::cout << std::endl << *temp_str << std::endl;
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

    //std::vector 

    Lexer lexer(source_code);
    Parser parser(lexer.getTokenizedSource(), lexer.getTokensInfo());


    // // Перебираем и ищем переменные, вызовы фунция, арифметические выражения и т.д
    // *temp_str = "";
    // for (char ch : source_code)
    // {
    //     if (ch == ';')
    //     {
    //         bool separated_word = false;
    //         uint16_t count_of_words = 0;
    //         for (char ch2 : *temp_str)
    //         {
    //             if ((ch2 == ' ' or ch2 == '\t' or ch2 == '\n' or ch2 == '\0' or ch2 == ';') and separated_word)
    //             {
    //                 ++count_of_words;
    //                 separated_word = false;
    //             }
    //             else if (ch2 != ' ' or ch2 != '\t' or ch2 != '\n' or ch2 != '\0' or ch2 != ';')
    //             {
    //                 separated_word = true;
    //             }
    //         }

    //         std::cout << *temp_str << " | " << count_of_words << std::endl;
    //         *temp_str = "";
    //     }
    //     else if (ch == '{' or ch == '}')
    //     {
    //         *temp_str = "";
    //     }
    //     else
    //     {
    //         *temp_str += ch;
    //     }

    // }


    delete temp_str;

// *_ var_type *_ func_name *_ ()
}


