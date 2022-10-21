#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
//std::map <std::string, std::string> SYMBOLS = {{"==", "sdf"}};

class Lexer{
private:
    //std::string lexedString = "";
    std::vector <std::vector<std::string>> TokenizedSource;
    std::vector <std::vector<std::string>> TokensInfo;
    bool   findB(std::string* beginIterator, std::string* endIterator, std::string findedObject);
    int8_t isNormString(std::string &str);
    bool   isNumber(std::string &str);


public:
    struct Expression{
        // Переменные и функции
        std::string var_type   = "";
        std::string var_name   = "";
        std::string func_name  = "";
        std::string exp_operator = "";
        std::string* func_args = nullptr; // Аргументы функций
        std::string* other_exp_tokens = nullptr; // Выражение 
        
        // Логические условия (if, else, while и т.п)
        std::string logical_type = "";
        std::string* logical_condition = nullptr;
    };

    Lexer(std::string sourceCode);
    //std::string getLexedString();
    std::vector<std::vector<std::string>> getTokenizedSource();
    std::vector<std::vector<std::string>> getTokensInfo();


private: // Т.к структура Expression объявлена ниже, чем первый private
    std::vector <Expression> lexedSourceCode;

};