#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

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
    Node* addNode(std::string type, std::string info = "", Node* mother_node = nullptr, bool add_to_mother = true);

    Node* recursionMathParser(
        std::vector<std::string> expression, std::vector<std::string> expression_info,
        Node *mother_node, std::string lmore = "", uint32_t lmore_index = 0
    );
    Node* recursionFuncParser(
        std::vector<std::string> expression, std::vector<std::string> expression_info , std::string name
    );
};
