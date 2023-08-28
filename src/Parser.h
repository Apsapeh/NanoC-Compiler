#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include "Lexer.h"

class Parser {
public:
    struct Node
    {
        std::string Type = "";
        std::string Info = "";
        std::vector<Node*> kind;
        Node *Mother = nullptr;
    };

    Parser(std::vector<std::vector<Lexer::Token>> &data);

    Node* getParsedProgram();
private:
    struct var_type {
        std::string old;
        std::string new;
    };

    struct var_info {
        std::string name;
        std::string type;
    };

    struct func_info {
        std::string name;
        std::vector<var_info> args;
        std::string return_type;
    };

    struct struct_info {
        std::string name;
        std::vector<var_info> members;
        std::vector<func_info> functions;
    };

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
