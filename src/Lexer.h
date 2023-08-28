#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
//std::map <std::string, std::string> SYMBOLS = {{"==", "sdf"}};

enum TokenType {
    RBR_BEG, RBR_END, SBR_BEG, SBR_END, BEGIN, END,
    SEPARATOR, DOT, ASSIGN, ADD, MINUS, MULT, DIV, MOD,
    ASSIGN_ADD, ASSIGN_MINUS, ASSIGN_MULT, ASSIGN_DIV, ASSIGN_MOD,
    LL, LM, LLQ, LMQ, LQ, LAND, LOR, LNOT,
    IF, ELSE, WHILE, VALUE, STRUCT
};

class Lexer{
private:
    /*std::vector<std::string> VarTypes = {
            "char", "short", "int", "long",
            "unsigned char", "unsigned short", "unsigned int", "unsigned long"
    };*/
    bool   isNumber(std::string &str);

public:
    struct Token {
        TokenType type;
        std::string info;
    };

    Lexer(std::vector<std::vector<std::string>> processed_source);
    std::vector<std::vector<Token>>& getLexedSource();


private: // Т.к структура Expression объявлена ниже, чем первый private
    std::vector <std::vector<Token>> TokenizedSource;
};