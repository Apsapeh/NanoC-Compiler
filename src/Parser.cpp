#include "Parser.h"
struct Node
{
    //std::string str = "";
    std::string Type = "";
    std::string Info = "";
    std::string Op1  = "";
    std::string Op2  = "";
    std::string Op3  = "";
    std::vector<Node*> kind;
    Node *Mother = nullptr;
};

void recursionNodePrint(Node *node, uint32_t &n_c);


Parser::Parser(std::vector<std::vector<std::string>> TokenizedSource,
               std::vector<std::vector<std::string>> TokensInfo)
{

    /*Node n1;
    n1.str = "Hello";
    Node n2;
    n2.str = "Hello 2";
    n2.kind.push_back(&n1);
    Node n3;
    n3.str = "Hello 3";
    n3.kind.push_back(&n2);*/

    //std::cout << n3.str << std::endl;
    //std::cout << n3.kind[0]->str << std::endl;
    //std::cout << n3.kind[0]->kind[0]->str << std::endl;

    //Node *tempN = &n3;

    /*while (true)
    {
        std::cout << tempN->str << std::endl;
        if (not tempN->kind.empty())
        {
            tempN = tempN->kind;
        }
        else
            break;
    }*/
    Node Program; Program.Type = "Program"; Program.Mother = &Program;
    Node *MotherNode = &Program;

    Node *tempN;
    bool isFunctionCall = false;
    for (uint32_t line_num=0; line_num < TokenizedSource.size(); ++line_num)
    {
        std::vector<std::string> &VecLine = TokenizedSource[line_num];
        std::vector<std::string> &VecLineInfo = TokensInfo[line_num];
        for (uint32_t token_num=0; token_num < VecLine.size(); ++token_num)
        {
            std::string &Token = VecLine[token_num];
            std::string &Info = VecLineInfo[token_num];

            //std::cout << Token << " ";
            if (Token == "FN_DEF" or Token == "FN_CALL" or Token == "IF" or Token == "ELSE" or Token == "WHILE")
            {
                tempN = new Node;
                tempN->Type = Token;
                if (Info != "~~~")
                    tempN->Info = Info;
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);
                MotherNode = tempN;

                if (Token == "FN_CALL")
                    isFunctionCall = true;
                //std::cout << Info << std::endl;
            }
            if (Token == "ARGS_BEGIN" or Token == "COND_BEGIN")
            {
                tempN = new Node;
                tempN->Type = "ARGS";
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);
                MotherNode = tempN;
            }
            if (Token == "ARGS_END" or Token == "COND_END" or Token == "END") {
                MotherNode = MotherNode->Mother;

                if (isFunctionCall)
                {
                    isFunctionCall = false;
                    MotherNode = MotherNode->Mother;
                }
            }
        }
        //std::cout << std::endl;
    }
    uint32_t kind_counter = 0;
    recursionNodePrint(&Program, kind_counter);
}


void recursionNodePrint(Node *node, uint32_t &k_c)
{
    std::cout << std::string(k_c, '\t') << node->Type /*<< " | " << node->Info << " | " << k_c*/ << std::endl;
    ++k_c;
    for (Node *kind : node->kind)
    {
        recursionNodePrint(kind, k_c);
    }
    --k_c;
}

