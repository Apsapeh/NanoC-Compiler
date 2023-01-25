#pragma once
#include <iostream>
#include <string>
#include <vector>

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
