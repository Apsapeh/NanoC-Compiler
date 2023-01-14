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
                temp_str.append(" ");
                if (((ascii_num > 60 and ascii_num < 63) or ascii_num == '!') and char_num+1 < sepr_str.length() and sepr_str[char_num+1] == '=')
                {
                    ++char_num;
                    temp_str += sepr_str[char_num-1];
                    temp_str += sepr_str[char_num];
                }
                else
                    temp_str.push_back(ascii_num);
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
            if (sepr_str[char_num] == ' ' or sepr_str[char_num] == ',' or char_num == sepr_str.length() - 1)
            {
                if (char_num == sepr_str.length() - 1) temp_str.push_back(sepr_str[char_num]);
                if (temp_str != "") {
                    temp_vec.push_back(temp_str);
                    temp_str.clear();
                }
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

        std::vector<std::string> tempVecTok;
        std::vector<std::string> tempVecTokInfo;

        ++string_num;
        Expression temp_expression;
        for (uint32_t str_num = 0; str_num < splitted.size(); ++str_num)
        {
            std::string exp_string = splitted[str_num];
            /*if (str_num > 0 and str_num < splitted.size())
                lexedString += " ";*/
            if (exp_string == "(")
            {
                if (not is_condition and not is_args and not is_function_call and not is_function_definition) {
                    tempVecTok.push_back("RBRAC_BEGIN");
                    tempVecTokInfo.push_back("~~~");
                }
                continue;
            }
            if (exp_string == ")")
            {
                is_args = false;

                if (str_num < splitted.size()-1 and splitted[str_num+1] == "{" and is_condition)
                {
                    //this->lexedString += "COND_END";
                    tempVecTok.push_back("COND_END");
                    tempVecTokInfo.push_back("~~~");
                    is_condition = false;
                    is_args = false;
                    continue;
                }
                else if ((str_num < splitted.size()-1 and splitted[str_num+1] == "{" and is_function_definition))
                {
                    //this->lexedString += "ARGS_END";
                    tempVecTok.push_back("ARGS_END");
                    tempVecTokInfo.push_back("~~~");
                    is_function_definition = false;
                    is_args = false;
                    continue;
                }
                else if (is_function_call)
                {
                    //this->lexedString += "ARGS_END";
                    tempVecTok.push_back("ARGS_END");
                    tempVecTokInfo.push_back("~~~");
                    is_function_call = false;
                    is_args = false;
                    continue;
                }
                else
                {
                    //this->lexedString += "ARGS_END";
                    tempVecTok.push_back("RBRAC_END");
                    tempVecTokInfo.push_back("~~~");
                    continue;
                }
            }

            if (exp_string == "{")
            {
                // BEGIN
                //this->lexedString += "BEGIN";
                tempVecTok.push_back("BEGIN");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "}")
            {
                // END
                //this->lexedString += "END";
                tempVecTok.push_back("END");
                tempVecTokInfo.push_back("~~~");
                continue;
            }

            if (exp_string == "=")
            {
                // ASSIGN
                //this->lexedString += "ASSIGN";
                tempVecTok.push_back("ASSIGN");
                tempVecTokInfo.push_back("~~~");
                second_exp_part = true;
                continue;
            }
            if (exp_string == "+")
            {
                // ADD
                //this->lexedString += "ADD";
                tempVecTok.push_back("ADD");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "-")
            {
                // MINUS
                //this->lexedString += "MINUS";
                tempVecTok.push_back("MINUS");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "*")
            {
                // MULTIPLE
                //this->lexedString += "MULTIPLE";
                tempVecTok.push_back("MULTIPLE");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "/")
            {
                // DIVIDE
                //this->lexedString += "DIVIDE";
                tempVecTok.push_back("DIVIDE");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "<")
            {
                // LLESS
                //this->lexedString += "LLESS";
                tempVecTok.push_back("LLESS");
                tempVecTokInfo.push_back("~~~");
                continue;
            }if (exp_string == ">")
            {
                // LMORE
                //this->lexedString += "LMORE";
                tempVecTok.push_back("LMORE");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "==")
            {
                // LEQUAL
                //this->lexedString += "LEQUAL";
                tempVecTok.push_back("LEQUAL");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "<=")
            {
                // LLESS_EQ
                //this->lexedString += "LLESS_EQ";
                tempVecTok.push_back("LLESS_EQ");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == ">=")
            {
                // LMORE_EQ
                //this->lexedString += "LMORE_EQ";
                tempVecTok.push_back("LMORE_EQ");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "!=")
            {
                // LNOT_EQ
                //this->lexedString += "LNOT_EQ";
                tempVecTok.push_back("LNOT_EQ");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "and" or exp_string == "&&")
            {
                // LAND
                //this->lexedString += "LAND";
                tempVecTok.push_back("LAND");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if (exp_string == "or" or exp_string == "||")
            {
                // LOR
                //this->lexedString += "LOR";
                tempVecTok.push_back("LOR");
                tempVecTokInfo.push_back("~~~");
                continue;
            }

            if(exp_string == "true")
            {
                // TRUE
                //this->lexedString += "TRUE";
                tempVecTok.push_back("TRUE");
                tempVecTokInfo.push_back("~~~");
                continue;
            }
            if(exp_string == "false")
            {
                // TRUE
                //this->lexedString += "FALSE";
                tempVecTok.push_back("FALSE");
                tempVecTokInfo.push_back("~~~");
                continue;
            }




            if (exp_string == "if")
            {
                if (str_num < splitted.size()-1 and splitted[str_num+1] == "(")
                {
                    // IF COND_BEGIN
                    //this->lexedString += "IF COND_BEGIN";
                    tempVecTok.push_back("IF");
                    tempVecTokInfo.push_back("~~~");
                    tempVecTok.push_back("COND_BEGIN");
                    tempVecTokInfo.push_back("~~~");
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
                    //this->lexedString += "ELSE";
                    tempVecTok.push_back("ELSE");
                    tempVecTokInfo.push_back("~~~");
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
                    //this->lexedString += "WHILE COND_BEGIN";
                    tempVecTok.push_back("WHILE");
                    tempVecTokInfo.push_back("~~~");
                    tempVecTok.push_back("COND_BEGIN");
                    tempVecTokInfo.push_back("~~~");
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
                    //this->lexedString += "VALUE@" + exp_string + "." + splitted[str_num+2];
                    tempVecTok.push_back("VALUE");
                    tempVecTokInfo.push_back(exp_string + "." + splitted[string_num+2]);
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
                    //this->lexedString += "VALUE@" + exp_string;
                    tempVecTok.push_back("VALUE");
                    tempVecTokInfo.push_back(exp_string);
                    continue;
                }
            }

            // Определяет тип переменной
            if (findB(std::begin(var_types), std::end(var_types), exp_string))
            {
                if (temp_expression.var_type == "" and not (is_args and is_function_definition))
                {
                    // VAR_TYPE@exp_string
                    //this->lexedString += "VAR_TYPE@" + exp_string;
                    tempVecTok.push_back("VAR_TYPE");
                    tempVecTokInfo.push_back(exp_string);
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
                    //this->lexedString += "VAR_TYPE@" + exp_string;
                    tempVecTok.push_back("VAR_TYPE");
                    tempVecTokInfo.push_back(exp_string);
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
                //this->lexedString += "VAR_NAME@" + exp_string;
                tempVecTok.push_back("VAR_NAME");
                tempVecTokInfo.push_back(exp_string);
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
                    //this->lexedString += "FN_DEF@" + exp_string + " ARGS_BEGIN";
                    tempVecTok.push_back("FN_DEF");
                    tempVecTokInfo.push_back(exp_string);
                    tempVecTok.push_back("ARGS_BEGIN");
                    tempVecTokInfo.push_back("~~~");
                    is_args = true;
                    is_function_definition = true;
                    continue;
                }
                else
                {
                    // Function call
                    //this->lexedString += "FN_CALL@" + exp_string + " ARGS_BEGIN";
                    tempVecTok.push_back("FN_CALL");
                    tempVecTokInfo.push_back(exp_string);
                    tempVecTok.push_back("ARGS_BEGIN");
                    tempVecTokInfo.push_back("~~~");
                    is_args = true;
                    is_function_call = true;
                    continue;
                }
            }

            if (exp_string == "(")
                exit(15);


            std::cout << "Lexer Error: Unknown token - \"" << exp_string << "\"" << std::endl;
            //exit(-1);
        }
        lexedSourceCode.push_back(temp_expression);
    
        /*if (&splitted != &separated_sourceCodeString[separated_sourceCodeString.size()-1])
            this->lexedString += "\n";*/

        TokenizedSource.push_back(tempVecTok);
        TokensInfo.push_back(tempVecTokInfo);
    }

    for (uint64_t n1=0; n1 < TokenizedSource.size(); ++n1)
    {
        std::cout << n1 << " -- ";
        for (uint64_t n2=0; n2 < TokenizedSource[n1].size(); ++n2)
        {
            std::cout << TokenizedSource[n1][n2] << " ";
            if (TokensInfo[n1][n2] != "~~~")
                std::cout << TokensInfo[n1][n2] << " ";
        }
        std::cout << "\n";
    }
    //std::cout << this->lexedString << std::endl;

}
std::vector<std::vector<std::string>> Lexer::getTokenizedSource()
{
    return TokenizedSource;
}

std::vector<std::vector<std::string>> Lexer::getTokensInfo()
{
    return TokensInfo;
}

/*std::string Lexer::getLexedString()
{
    return this->lexedString;
}*/


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
