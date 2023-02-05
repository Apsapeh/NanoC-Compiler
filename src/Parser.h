#pragma once
#include <iostream>
#include <string>
#include <vector>

typedef uint64_t u_int64_t;
typedef uint32_t u_int32_t;
typedef uint16_t u_int16_t;
typedef uint8_t u_int8_t;

class Parser {

public:
    struct Node
    {
        //std::string str = "";
        std::string Type = "";
        std::string Info = "";
        std::vector<Node*> kind;
        Node *Mother = nullptr;
    };

    Parser(std::vector<std::vector<std::string>> TokenizedSource,
           std::vector<std::vector<std::string>> TokensInfo);

    Node* getParsedProgram();
private:
    Node* ParsedProgram;
};
