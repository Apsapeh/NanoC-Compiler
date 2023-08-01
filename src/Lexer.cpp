#include "main.h"
#include "Error.h"
#include "Lexer.h"

static void add_info_to_temp_vectors(std::vector<std::string> &tok_vec, std::vector<std::string> &tok_info_vec, std::string token, std::string token_info);

Lexer::Lexer(std::vector<std::vector<std::string>> processed_source)
{
    // Лексинг строк
    bool is_function_definition = false;
    int is_function_call = 0;
    bool is_args = false;
    uint64_t string_num = 0;
    for (std::vector <std::string> &splitted : processed_source)
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
                else if (is_function_definition) {
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

            else if (exp_string == "[") {
                ADD_TOKEN_TO_TMPVEC("SBRAC_BEGIN", "~~~");
            }
            else if (exp_string == "]") {
                ADD_TOKEN_TO_TMPVEC("SBRAC_END", "~~~");
            }

            else if (exp_string == ",")
                ADD_TOKEN_TO_TMPVEC("SEPARATOR", "~~~");

            else if (exp_string == "{")
                ADD_TOKEN_TO_TMPVEC("BEGIN", "~~~");
            else if (exp_string == "}")
                ADD_TOKEN_TO_TMPVEC("END", "~~~");

            else if (exp_string == "="){
                ADD_TOKEN_TO_TMPVEC("ASSIGN", "~~~");
                second_exp_part = true;
            }
            else if (exp_string == "+=" or exp_string == "-=" or exp_string == "*=" or exp_string == "/=" or exp_string == "%=") {
                if (str_num-1 >= 0 and isNormString(splitted[str_num-1]) == 1) {
                    ADD_TOKEN_TO_TMPVEC("ASSIGN", "~~~");
                    ADD_TOKEN_TO_TMPVEC("VAR_NAME", splitted[str_num-1]);
                    if (exp_string == "+=")
                        ADD_TOKEN_TO_TMPVEC("ADD", "~~~");
                    else if (exp_string == "-=")
                        ADD_TOKEN_TO_TMPVEC("MINUS", "~~~");
                    else if (exp_string == "*=")
                        ADD_TOKEN_TO_TMPVEC("MULTIPLE", "~~~");
                    else if (exp_string == "/=")
                        ADD_TOKEN_TO_TMPVEC("DIVIDE", "~~~");
                    else if (exp_string == "%=")
                        ADD_TOKEN_TO_TMPVEC("MOD", "~~~");
                }
                else {

                }
            }
            else if (exp_string == "+")
                ADD_TOKEN_TO_TMPVEC("ADD", "~~~");
            else if (exp_string == "-")
                ADD_TOKEN_TO_TMPVEC("MINUS", "~~~");
            else if (exp_string == "*")
                ADD_TOKEN_TO_TMPVEC("MULTIPLE", "~~~");
            else if (exp_string == "/")
                ADD_TOKEN_TO_TMPVEC("DIVIDE", "~~~");
            else if (exp_string == "%")
                ADD_TOKEN_TO_TMPVEC("MOD", "~~~");

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
                if (str_num < splitted.size()-2 and (splitted[str_num+1] == "." and !isNumber(splitted[str_num+2]))) {
                    std::cout << "Syntax error: \"" << splitted[str_num+2] << "\" - is not a number" << std::endl;
                    exit(-1);
                }
                else if (str_num < splitted.size()-1 and splitted[str_num+1] == ".") {
                    std::cout << "Error: There must be a number after the dot [" << string_num << "]\n";
                    exit(-1);
                }
                else {
                    if (exp_string[0] == '.')
                        ADD_TOKEN_TO_TMPVEC("VALUE", "0" + exp_string);
                    else if (exp_string[exp_string.size()-1] == '.')
                        ADD_TOKEN_TO_TMPVEC("VALUE", exp_string + "0");
                    else
                        ADD_TOKEN_TO_TMPVEC("VALUE", exp_string);
                }
            }

            else if (exp_string.size() > 1 and exp_string[0] == '"' and exp_string[exp_string.size()-1] == '"') {
                ADD_TOKEN_TO_TMPVEC("VALUE", std::string(exp_string.begin()+1, exp_string.end()-1));
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
            //std::cout << exp_string << "#";

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
                std::cout << "(" << TokensInfo[n1][n2] << ") ";
        }
        std::cout << "\n";
    }
    //std::cout << this->lexedString << std::endl;

}
std::vector<std::vector<std::string>> Lexer::getTokenizedSource() {
    return TokenizedSource;
}

std::vector<std::vector<std::string>> Lexer::getTokensInfo() {
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
    uint32_t dot_count = 0;
    for (char ascii_num : str)
    {
        if (ascii_num == '.')
            ++dot_count;
        else if (not (ascii_num > 47 and ascii_num < 58))
            return false;
    }

    if (dot_count <= 1)
        return true;
    else
        Error::throwError("The number \"" + str + "\" contains too many dots");
}


static void add_info_to_temp_vectors(std::vector<std::string> &tok_vec, std::vector<std::string> &tok_info_vec, std::string token, std::string token_info)
{
    tok_vec.push_back(token);
    tok_info_vec.push_back(token_info);
}
