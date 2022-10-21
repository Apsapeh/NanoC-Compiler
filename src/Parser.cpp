#include "Parser.h"
struct Node
{
    std::string str = "";
    Node *kind = nullptr;
};



Parser::Parser(std::vector<std::vector<std::string>> TokenizedSource,
               std::vector<std::vector<std::string>> TokensInfo)
{

    Node n1;
    n1.str = "Hello";
    Node n2;
    n2.str = "Hello 2";
    n2.kind = &n1;
    Node n3;
    n3.str = "Hello 3";
    n3.kind = &n2;

    std::cout << n3.str << std::endl;
    std::cout << n3.kind->str << std::endl;
    std::cout << n3.kind->kind->str << std::endl;

    Node *tempN = &n3;

    while (true)
    {
        std::cout << tempN->str << std::endl;
        if (tempN->kind != nullptr)
        {
            tempN = tempN->kind;
        }
        else
            break;
    }

    for (uint32_t line_num=0; line_num < TokenizedSource.size(); ++line_num)
    {
        std::vector<std::string> &VecLine = TokenizedSource[line_num];
        std::vector<std::string> &VecLineInfo = TokenizedSource[line_num];
        for (uint32_t token_num=0; token_num < VecLine.size(); ++token_num)
        {
            std::string &Token = VecLine[token_num];
            std::string &Info = VecLine[token_num];

            //std::cout << Token << " ";
        }
        //std::cout << std::endl;
    }
}
