#include "main.h"
#include "Lexer.h"


Lexer::Lexer(std::string sourceCode)
{
    // Деление исходного кода на строки
    std::string temp_str = "";
    std::vector <std::string> separated_sourceCode;
    for (unsigned long long char_num = 0; char_num < sourceCode.length(); ++char_num)
    {
        // Парсинг выражений, не заканчивающихся на ";"
        if (sourceCode[char_num] == '/' and (char_num < sourceCode.length()-1 and sourceCode[char_num+1] == '/'))
        {
            while (sourceCode[char_num] != '\n' and char_num < sourceCode.length())
                ++char_num;

        }
        if (sourceCode[char_num] == '/' and (char_num < sourceCode.length()-1 and sourceCode[char_num+1] == '*')) {
            while (char_num < sourceCode.length()-1 and not (sourceCode[char_num] == '*' and sourceCode[char_num+1] == '/'))
                ++char_num;
            char_num += 2;
        }

        //TODO: Здесь пропускает строки начинающиеся с решётки, чтобы компилтор на ругался на define
        if (sourceCode[char_num] == '#')
        {
            while (sourceCode[char_num] != '\n' and char_num < sourceCode.length())
                ++char_num;
        }

        // Основной парсинг
        if (sourceCode[char_num] == ';')
        {
            separated_sourceCode.push_back(temp_str);
            temp_str = "";
        }
        else if (sourceCode[char_num] == '{' or sourceCode[char_num] == '}')
        {
            temp_str += sourceCode[char_num];
            separated_sourceCode.push_back(temp_str);
            temp_str = "";
        }
        else
        {
            if (sourceCode[char_num] == '\n' or sourceCode[char_num] == '\t')
                temp_str += " ";
            else
                temp_str += sourceCode[char_num];
        }
    }
    
    // Удалет лишние пробелы и добавляет пробелы между символами != цифрам, маленьким и большим буквам (ASCII)
    for (std::string &sepr_str : separated_sourceCode)
    {
        // Добавляет пробелы между определёнными символами
        temp_str = "";
        for(unsigned long long char_num = 0; char_num < sepr_str.length(); ++char_num)
        {
            char ascii_num = sepr_str[char_num];
            if (not ((ascii_num > 47 and ascii_num < 58) or (ascii_num > 64 and ascii_num < 91) or
                     (ascii_num > 96 and ascii_num < 123)) and ascii_num != ' ')
            {
                temp_str.append(" ").push_back(ascii_num);
                temp_str.append(" ");
            }
            else
                temp_str += sepr_str[char_num];
        }
        sepr_str = temp_str;
        
        // Удаляет лишние пробелы
        temp_str = "";
        unsigned int space_count = 0;
        for(unsigned long long char_num = 0; char_num < sepr_str.length(); ++char_num)
        {
            sepr_str[char_num]==' ' ? ++space_count : space_count=0;
            if (space_count < 2)
                temp_str += sepr_str[char_num];
        }
        
        if (temp_str[0] == ' ') 
            temp_str.erase(temp_str.begin());
        if (temp_str[temp_str.length()-1] == ' ')
            temp_str.erase(temp_str.end() - 1);

        sepr_str = temp_str;
    }

    // Разделение строки по пробелам
    temp_str.clear();
    std::vector <std::vector<std::string>> separated_sourceCodeString;
    for (std::string &sepr_str : separated_sourceCode)
    {
        std::vector <std::string> temp_vec;
        for (unsigned long long char_num = 0; char_num < sepr_str.length(); ++char_num)
        {
            if (sepr_str[char_num] == ' ' or char_num == sepr_str.length() - 1)
            {
                if (char_num == sepr_str.length() - 1) temp_str.push_back(sepr_str[char_num]);
                temp_vec.push_back(temp_str);
                temp_str.clear();
            }
            else
                temp_str.push_back(sepr_str[char_num]);
        }
        separated_sourceCodeString.push_back(temp_vec);
    }


    // Лексинг строк
    bool is_function_definition = false;
    bool is_function_call = false;
    bool is_args = false;
    uint64_t string_num = 0;
    for (std::vector <std::string> &splitted : separated_sourceCodeString)
    {
        bool second_exp_part = false;
        bool is_condition = false;

        ++string_num;
        Expression temp_expression;
        for (uint32_t str_num = 0; str_num < splitted.size(); ++str_num)
        {
            std::string exp_string = splitted[str_num];
            if (str_num > 0 and str_num < splitted.size())
                lexedString += " ";

            if (exp_string == "(")
            {
                continue;
            }
            if (exp_string == ")")
            {
                is_args = false;

                if (str_num < splitted.size()-1 and splitted[str_num+1] == "{" and is_condition)
                {
                    this->lexedString += "COND_END";
                    is_condition = false;
                    is_args = false;
                    continue;
                }

                if ((str_num < splitted.size()-1 and splitted[str_num+1] == "{" and is_function_definition))
                {
                    this->lexedString += "ARGS_END";
                    is_function_definition = false;
                    is_args = false;
                    continue;
                }

                if (is_function_call)
                {
                    this->lexedString += "ARGS_END";
                    is_function_call = false;
                    is_args = false;
                    continue;
                }
            }

            if (exp_string == "{")
            {
                // BEGIN
                this->lexedString += "BEGIN";
                continue;
            }
            if (exp_string == "}")
            {
                // END
                this->lexedString += "END";
                continue;
            }

            if (exp_string == "=")
            {
                // ASSIGN
                this->lexedString += "ASSIGN";
                second_exp_part = true;
                continue;
            }
            if (exp_string == "+")
            {
                // ADD
                this->lexedString += "ADD";
                continue;
            }
            if (exp_string == "-")
            {
                // MINUS
                this->lexedString += "BEGIN";
                continue;
            }
            if (exp_string == "*")
            {
                // MULTIPLE
                this->lexedString += "MULTIPLE";
                continue;
            }
            if (exp_string == "/")
            {
                // DIVIDE
                this->lexedString += "DIVIDE";
                continue;
            }
            if (exp_string == "<")
            {
                // LLESS
                this->lexedString += "LLESS";
                continue;
            }if (exp_string == ">")
            {
                // LMORE
                this->lexedString += "LMORE";
                continue;
            }
            if (exp_string == "==")
            {
                // LEQUAL
                this->lexedString += "LEQUAL";
                continue;
            }
            if (exp_string == "<=")
            {
                // LLESS_EQ
                this->lexedString += "LLESS_EQ";
                continue;
            }
            if (exp_string == ">=")
            {
                // LMORE_EQ
                this->lexedString += "LMORE_EQ";
                continue;
            }
            if (exp_string == "!=")
            {
                // LNOT_EQ
                this->lexedString += "LNOT_EQ";
                continue;
            }
            if (exp_string == "and" or exp_string == "&&")
            {
                // LAND
                this->lexedString += "LAND";
                continue;
            }
            if (exp_string == "or" or exp_string == "||")
            {
                // LOR
                this->lexedString += "LOR";
                continue;
            }

            if(exp_string == "true")
            {
                // TRUE
                this->lexedString += "TRUE";
                continue;
            }
            if(exp_string == "false")
            {
                // TRUE
                this->lexedString += "FALSE";
                continue;
            }




            if (exp_string == "if")
            {
                if (str_num < splitted.size()-1 and splitted[str_num+1] == "(")
                {
                    // IF COND_BEGIN
                    this->lexedString += "IF COND_BEGIN";
                    is_args = true;
                    is_condition = true;
                    ++str_num;
                    continue;
                }
                else if (str_num < splitted.size()-1 and splitted[str_num+1] != "(")
                {
                    std::cout << "Error: The 'if' block is declared without a condition [" << str_num << "]\n";
                    exit(-1);
                }
            }
            if (exp_string == "else")
            {
                if (splitted.size() == 2 and splitted[1] == "{")
                {
                    this->lexedString += "ELSE";
                    continue;
                }
                else
                {
                    if (splitted.size() > 2)
                    {
                        std::cout << "Syntax error [" << string_num << "]\n";
                        exit(-1);
                    }
                    if (splitted.size() < 2 or (splitted.size() == 2 and splitted[1] != "{"))
                    {
                        std::cout << "Error: The \"else\" block must contain an expression (\"{...}\" - not found)\n";
                        exit(-1);
                    }
                }
            }

            if (exp_string == "while")
            {
                if (str_num < splitted.size()-1 and splitted[str_num+1] == "(")
                {
                    // IF COND_BEGIN
                    this->lexedString += "WHILE COND_BEGIN";
                    is_args = true;
                    is_condition = true;
                    ++str_num;
                    continue;
                }
                else if (str_num < splitted.size()-1 and splitted[str_num+1] != "(")
                {
                    std::cout << "Error: The 'while' block is declared without a condition [" << str_num << "]\n";
                    exit(-1);
                }
            }

            // if (exp_string == ".")
            // {
            //     if ((str_num > 0 and isNumber(splitted[str_num-1])) and 
            //         (str_num < splitted.size()-1 and isNumber(splitted[str_num+1])))
            //     {
                    

            //     }
            // }

            // Определяет числовую константу
            if (isNumber(exp_string))
            {
                if (str_num < splitted.size()-2 and (splitted[str_num+1] == "." and isNumber(splitted[str_num+2])))
                {
                    // VALUE@exp_string + "." + splitted[str_num+2]
                    this->lexedString += "VALUE@" + exp_string + "." + splitted[str_num+2];
                    str_num += 2;
                    continue;
                }
                else if (str_num < splitted.size()-2 and (splitted[str_num+1] == "." and !isNumber(splitted[str_num+2])))
                {
                    std::cout << "Syntax error: \"" << splitted[str_num+2] << "\" - is not a number" << std::endl;
                    exit(-1);
                }
                else if (str_num < splitted.size()-1 and splitted[str_num+1] == ".")
                {
                    std::cout << "Error: There must be a number after the dot [" << string_num << "]\n";
                    exit(-1);
                }
                else
                {
                    // VALUE@exp_string
                    this->lexedString += "VALUE@" + exp_string;
                    continue;
                }
            }

            // Определяет тип переменной
            if (findB(std::begin(var_types), std::end(var_types), exp_string))
            {
                if (temp_expression.var_type == "" and not (is_args and is_function_definition))
                {
                    // VAR_TYPE@exp_string
                    this->lexedString += "VAR_TYPE@" + exp_string;
                    temp_expression.var_type = exp_string;
                    continue;
                }
                else if (temp_expression.var_type != "" and not (is_args or is_function_definition))
                {
                    
                    std::cout << "Error: Multiple definition of variable type [" << string_num << "]" << std::endl;
                    exit(-1);
                }
                else if (is_args or is_function_definition)
                {
                    // VAR_TYPE@exp_string
                    this->lexedString += "VAR_TYPE@" + exp_string;
                    continue;
                }
                else
                {
                    std::cout << "Syntax error [" << string_num << "]" << std::endl;
                    exit(-1);
                }
            }

            // Определяет имя переменной
            if (isNormString(exp_string) == 1 and ((str_num < splitted.size()-1 and splitted[str_num+1] != "(") or
                str_num == splitted.size()-1))
            {

                // VAR_NAME@exp_string
                this->lexedString += "VAR_NAME@" + exp_string;
                temp_expression.var_name = exp_string;
                continue;
            }
            else if (isNormString(exp_string) != 1)
            {
                int8_t code = isNormString(exp_string);
                
                if (code == -1)
                {
                    std::cout << "Error: variable name starated from number" << std::endl;
                }
                else if (code == -2)
                    std::cout << "Error: invalid character in the variable name \"" << exp_string << "\" ["
                              << string_num << "]" << std::endl;

                std::cout << exp_string;
                exit(-1);
            }

            // Function
            if (isNormString(exp_string) == 1 and (str_num < splitted.size()-1 and splitted[str_num+1] == "("))
            {
                if (str_num > 0 and findB(std::begin(var_types), std::end(var_types), splitted[str_num-1]))
                {
                    // Function definition
                    this->lexedString += "FN_DEF@" + exp_string + " ARGS_BEGIN";
                    is_args = true;
                    is_function_definition = true;
                    continue;
                }
                else
                {
                    // Function call
                    this->lexedString += "FN_CALL@" + exp_string + " ARGS_BEGIN";
                    is_args = true;
                    is_function_call = true;
                    continue;
                }
            }
            


            std::cout << "Lexer Error: Unknown token - \"" << exp_string << "\"" << std::endl;
            //exit(-1);
        }
        lexedSourceCode.push_back(temp_expression);
    
        if (&splitted != &separated_sourceCodeString[separated_sourceCodeString.size()-1])
            this->lexedString += "\n";
    }

    //std::cout << this->lexedString << std::endl;

}

std::string Lexer::getLexedString()
{
    return this->lexedString;
}


// Проверяет наличие элемента в массиве
bool Lexer::findB(std::string* beginIterator, std::string* endIterator, std::string findedObject)
{
    for (std::string* iterator = beginIterator; iterator != endIterator; ++iterator)
    {
        if (*iterator == findedObject)
            return true;
    }
    return false;
}

int8_t Lexer::isNormString(std::string &str)
{
    if (str[0] > 47 and str[0] < 58)
        return -1;

    for(char ascii_num : str)
    {
        if (not ((ascii_num > 47 and ascii_num < 58) or (ascii_num > 64 and ascii_num < 91) or
                 (ascii_num > 96 and ascii_num < 123) or ascii_num == '_') )
        {
            return -2;
        }
    }

    return 1;
}

bool Lexer::isNumber(std::string &str)
{
    for (char ascii_num : str)
    {
        if (not (ascii_num > 47 and ascii_num < 58))
            return false;
    }

    return true;
}
