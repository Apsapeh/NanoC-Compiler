#include "main.h"
#include "Lexer.h"

static void add_info_to_temp_vectors(std::vector<std::string> &tok_vec, std::vector<std::string> &tok_info_vec, std::string token, std::string token_info);

//#define ADD_TOKEN_TO_TMPVEC(TOKEN, INFO) add_info_to_temp_vectors(tempVecTok, tempVecTokInfo, TOKEN, INFO)

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
        if (sourceCode[char_num] == '#') {
            while (sourceCode[char_num] != '\n' and char_num < sourceCode.length())
                ++char_num;
        }

        // Основной парсинг
        if (sourceCode[char_num] == ';') {
            separated_sourceCode.push_back(temp_str);
            temp_str = "";
        }
        else if (sourceCode[char_num] == '{' or sourceCode[char_num] == '}') {
            temp_str += sourceCode[char_num];
            separated_sourceCode.push_back(temp_str);
            temp_str = "";
        }
        else {
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
        for (unsigned long long char_num = 0; char_num < sepr_str.length(); ++char_num)
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
                if (sepr_str[char_num] == ',')
                    temp_vec.push_back("SEPARATOR");
            }
            else
                temp_str.push_back(sepr_str[char_num]);
        }
        separated_sourceCodeString.push_back(temp_vec);
    }


    // Простейший препроцессинг
    for (std::vector <std::string> &splitted : separated_sourceCodeString) {
        for (uint32_t str_num = 0; str_num < splitted.size(); ++str_num) {
            if (splitted[str_num] == "true") {
                splitted[str_num] = "1";
            }
            else if (splitted[str_num] == "false") {
                splitted[str_num] = "0";
            }
        }
    }


    // Лексинг строк
    bool is_function_definition = false;
    int is_function_call = 0;
    bool is_args = false;
    uint64_t string_num = 0;
    for (std::vector <std::string> &splitted : separated_sourceCodeString)
    {
        bool second_exp_part = false;
        bool is_condition = false;

        #define ADD_TOKEN_TO_TMPVEC(TOKEN, INFO) add_info_to_temp_vectors(tempVecTok, tempVecTokInfo, TOKEN, INFO)
        std::vector<std::string> tempVecTok;
        std::vector<std::string> tempVecTokInfo;

        ++string_num;
        Expression temp_expression;
        for (uint32_t str_num = 0; str_num < splitted.size(); ++str_num)
        {
            std::string exp_string = splitted[str_num];
            if (exp_string == "(") {
                if (not is_condition and not is_args and is_function_call == 0 and not is_function_definition)
                    ADD_TOKEN_TO_TMPVEC("RBRAC_BEGIN", "~~~");

            }
            else if (exp_string == ")") {
                is_args = false;

                if (str_num < splitted.size()-1 and splitted[str_num+1] == "{" and is_condition) {
                    ADD_TOKEN_TO_TMPVEC("COND_END", "~~~");
                    is_condition = false;
                    is_args = false;
                }
                else if ((str_num < splitted.size()-1 and splitted[str_num+1] == "{" and is_function_definition)) {
                    ADD_TOKEN_TO_TMPVEC("ARGS_END", "~~~");
                    is_function_definition = false;
                    is_args = false;
                }
                else if (is_function_call != 0) {
                    ADD_TOKEN_TO_TMPVEC("ARGS_END", "~~~");
                    --is_function_call;
                    is_args = false;
                }
                else
                    ADD_TOKEN_TO_TMPVEC("RBRAC_END", "~~~");
            }

            else if (exp_string == "SEPARATOR")
                ADD_TOKEN_TO_TMPVEC("SEPARATOR", "~~~");

            else if (exp_string == "{")
                ADD_TOKEN_TO_TMPVEC("BEGIN", "~~~");
            else if (exp_string == "}")
                ADD_TOKEN_TO_TMPVEC("END", "~~~");

            else if (exp_string == "="){
                ADD_TOKEN_TO_TMPVEC("ASSIGN", "~~~");
                second_exp_part = true;
            }
            else if (exp_string == "+")
                ADD_TOKEN_TO_TMPVEC("ADD", "~~~");
            else if (exp_string == "-")
                ADD_TOKEN_TO_TMPVEC("MINUS", "~~~");
            else if (exp_string == "*")
                ADD_TOKEN_TO_TMPVEC("MULTIPLE", "~~~");
            else if (exp_string == "/")
                ADD_TOKEN_TO_TMPVEC("DIVIDE", "~~~");

            else if (exp_string == "<")
                ADD_TOKEN_TO_TMPVEC("LLESS", "~~~");
            else if (exp_string == ">")
                ADD_TOKEN_TO_TMPVEC("LMORE", "~~~");
            else if (exp_string == "==")
                ADD_TOKEN_TO_TMPVEC("LEQUAL", "~~~");
            else if (exp_string == "<=")
                ADD_TOKEN_TO_TMPVEC("LLESS_EQ", "~~~");
            else if (exp_string == ">=")
                ADD_TOKEN_TO_TMPVEC("LMORE_EQ", "~~~");
            else if (exp_string == "!=")
                ADD_TOKEN_TO_TMPVEC("LNOT_EQ", "~~~");
            else if (exp_string == "and" or exp_string == "&&")
                ADD_TOKEN_TO_TMPVEC("LAND", "~~~");
            else if (exp_string == "or" or exp_string == "||")
                ADD_TOKEN_TO_TMPVEC("LOR", "~~~");
            else if (exp_string == "not" or exp_string == "!")
                ADD_TOKEN_TO_TMPVEC("LNOT", "~~~");

            else if (exp_string == "break")
                ADD_TOKEN_TO_TMPVEC("BREAK", "~~~");


            else if (exp_string == "if")
            {
                if (str_num < splitted.size()-1 and splitted[str_num+1] == "(") {
                    // IF COND_BEGIN
                    ADD_TOKEN_TO_TMPVEC("IF", "~~~");
                    ADD_TOKEN_TO_TMPVEC("COND_BEGIN", "~~~");
                    is_args = true;
                    is_condition = true;
                    ++str_num;
                }
                else if (str_num < splitted.size()-1 and splitted[str_num+1] != "(") {
                    std::cout << "Error: The 'if' block is declared without a condition [" << str_num << "]\n";
                    exit(-1);
                }
            }
            else if (exp_string == "else")
            {
                if (splitted.size() == 2 and splitted[1] == "{")
                    ADD_TOKEN_TO_TMPVEC("ELSE", "~~~");
                else
                {
                    if (splitted.size() > 2) {
                        std::cout << "Syntax error [" << string_num << "]\n";
                        exit(-1);
                    }
                    if (splitted.size() < 2 or (splitted.size() == 2 and splitted[1] != "{")) {
                        std::cout << "Error: The \"else\" block must contain an expression (\"{...}\" - not found)\n";
                        exit(-1);
                    }
                }
            }

            else if (exp_string == "while")
            {
                if (str_num < splitted.size()-1 and splitted[str_num+1] == "(") {
                    ADD_TOKEN_TO_TMPVEC("WHILE", "~~~");
                    ADD_TOKEN_TO_TMPVEC("COND_BEGIN", "~~~");
                    is_args = true;
                    is_condition = true;
                    ++str_num;
                }
                else if (str_num < splitted.size()-1 and splitted[str_num+1] != "(") {
                    std::cout << "Error: The 'while' block is declared without a condition [" << str_num << "]\n";
                    exit(-1);
                }
            }

            // Определяет числовую константу
            else if (isNumber(exp_string))
            {
                if (str_num < splitted.size()-2 and (splitted[str_num+1] == "." and isNumber(splitted[str_num+2]))) {
                    ADD_TOKEN_TO_TMPVEC("VALUE", exp_string + "." + splitted[string_num+2]);
                    str_num += 2;
                }
                else if (str_num < splitted.size()-2 and (splitted[str_num+1] == "." and !isNumber(splitted[str_num+2]))) {
                    std::cout << "Syntax error: \"" << splitted[str_num+2] << "\" - is not a number" << std::endl;
                    exit(-1);
                }
                else if (str_num < splitted.size()-1 and splitted[str_num+1] == ".") {
                    std::cout << "Error: There must be a number after the dot [" << string_num << "]\n";
                    exit(-1);
                }
                else {
                    ADD_TOKEN_TO_TMPVEC("VALUE", exp_string);
                    continue;
                }
            }

            // Определяет тип переменной
            else if (findB(std::begin(var_types), std::end(var_types), exp_string))
            {
                if (temp_expression.var_type == "" and not (is_args and is_function_definition)) {
                    ADD_TOKEN_TO_TMPVEC("VAR_TYPE", exp_string);
                    temp_expression.var_type = exp_string;
                }
                else if (temp_expression.var_type != "" and not (is_args or is_function_definition)){
                    std::cout << "Error: Multiple definition of variable type [" << string_num << "]" << std::endl;
                    exit(-1);
                }
                else if (is_args or is_function_definition)
                    ADD_TOKEN_TO_TMPVEC("VAR_TYPE", exp_string);

                else {
                    std::cout << "Syntax error [" << string_num << "]" << std::endl;
                    exit(-1);
                }
            }

            // Определяет имя переменной
            else if (isNormString(exp_string) == 1 and ((str_num < splitted.size()-1 and splitted[str_num+1] != "(") or
                str_num == splitted.size()-1))
            {
                ADD_TOKEN_TO_TMPVEC("VAR_NAME", exp_string);
                temp_expression.var_name = exp_string;
            }
            else if (isNormString(exp_string) != 1)
            {
                int8_t code = isNormString(exp_string);

                if (code == -1)
                    std::cout << "Error: variable name starated from number" << std::endl;
                else if (code == -2)
                    std::cout << "Error: invalid character in the variable name \"" << exp_string << "\" ["
                              << string_num << "]" << std::endl;
                exit(-1);
            }

            // Function
            else if (isNormString(exp_string) == 1 and (str_num < splitted.size()-1 and splitted[str_num+1] == "("))
            {
                if (str_num > 0 and findB(std::begin(var_types), std::end(var_types), splitted[str_num-1])){
                    // Function definition
                    ADD_TOKEN_TO_TMPVEC("FN_DEF", exp_string);
                    ADD_TOKEN_TO_TMPVEC("ARGS_BEGIN", "~~~");
                    is_args = true;
                    is_function_definition = true;
                }
                else{
                    // Function call
                    ADD_TOKEN_TO_TMPVEC("FN_CALL", exp_string);
                    ADD_TOKEN_TO_TMPVEC("ARGS_BEGIN", "~~~");
                    is_args = true;
                    ++is_function_call;
                }
            }

            else if (exp_string == "(")
                exit(15);

            else {
                std::cout << "Lexer Error: Unknown token - \"" << exp_string << "\"" << std::endl;
                exit(1);
            }

        }
        lexedSourceCode.push_back(temp_expression);

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

static void add_info_to_temp_vectors(std::vector<std::string> &tok_vec, std::vector<std::string> &tok_info_vec, std::string token, std::string token_info)
{
    tok_vec.push_back(token);
    tok_info_vec.push_back(token_info);
}
