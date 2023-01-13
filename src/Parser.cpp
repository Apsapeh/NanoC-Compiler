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

unsigned int findToken(std::vector<std::string> &vec, std::string str);
static void recursionNodePrint (Node *node, uint32_t &n_c);
Node* recursionMathParser(std::vector<std::string> expression);


Parser::Parser(std::vector<std::vector<std::string>> TokenizedSource,
               std::vector<std::vector<std::string>> TokensInfo)
{
    Node Program; Program.Type = "Program"; Program.Mother = &Program;
    Node *MotherNode = &Program;

    Node *tempN;
    bool isFunctionCall = false;
    for (uint32_t line_num=0; line_num < TokenizedSource.size(); ++line_num)
    {
        std::vector<std::string> &VecLine = TokenizedSource[line_num];
        std::vector<std::string> &VecLineInfo = TokensInfo[line_num];
        //parseExp(TokenizedSource[line_num], TokensInfo[line_num]);
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

                if (Token == "FN_DEF" and token_num > 0 and *(&Token-1) == "VAR_TYPE")
                {
                    tempN = new Node;
                    tempN->Type = "RETURNING_TYPE";
                    tempN->Info = *(&Info-1);
                    tempN->Mother = MotherNode;
                    MotherNode->kind.push_back(tempN);
                }
                else if (Token == "FN_DEF" and not (token_num > 0 and *(&Token-1) == "VAR_TYPE"))
                {
                    std::cout << "Error: The return type of the function is undefined\n";
                    exit(-1);
                }

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

            if (Token == "VAR_TYPE" and MotherNode->Type == "ARGS") {
                tempN = new Node;
                tempN->Type = Token;
                tempN->Info = Info;
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);
                MotherNode = tempN;

                if (not (token_num < VecLine.size() - 1 and *(&Token+1) == "VAR_NAME"))
                {
                    std::cout << "Error: Argument name is not set\n";
                    exit(-1);
                }

                tempN = new Node;
                tempN->Type = *(&Token+1);
                tempN->Info = *(&Info+1);
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);
                MotherNode = MotherNode->Mother;
                ++token_num;
            }

            if ((Token == "VAR_NAME" or Token == "VALUE") and MotherNode->Type == "ARGS") {
                tempN = new Node;
                tempN->Type = Token;
                tempN->Info = Info;
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);
            }

            if (Token == "ASSIGN") {
                std::cout << line_num << std::endl;
                tempN = new Node;
                tempN->Type = Token;
                tempN->Mother = MotherNode;
                tempN->kind.push_back(recursionMathParser(std::vector<std::string> (VecLine.begin()+token_num+1, VecLine.end())));
                uint32_t a = 0;
                recursionNodePrint(tempN->kind[0], a);
                token_num = VecLine.size();
            }

        }
    }
    uint32_t kind_counter = 0;
    //recursionNodePrint(&Program, kind_counter);
}

/*void Parser::parseExp(std::vector<std::string> &VecLine, std::vector<std::string> &VecLineInfo) {
    for (uint32_t token_num=0; token_num < VecLine.size(); ++token_num)
    {
        //parseExp(VecLine[token_num], VecLineInfo[token_num]);
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

            if (Token == "FN_DEF" and token_num > 0 and *(&Token-1) == "VAR_TYPE")
            {
                tempN = new Node;
                tempN->Type = "RETURNING_TYPE";
                tempN->Info = *(&Info-1);
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);
            }
            else if (Token == "FN_DEF" and not (token_num > 0 and *(&Token-1) == "VAR_TYPE"))
            {
                std::cout << "Error: The return type of the function is undefined\n";
                exit(-1);
            }

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

        if (Token == "VAR_TYPE" and MotherNode->Type == "ARGS") {
            tempN = new Node;
            tempN->Type = Token;
            tempN->Info = Info;
            tempN->Mother = MotherNode;
            MotherNode->kind.push_back(tempN);
            MotherNode = tempN;

            if (not (token_num < VecLine.size() - 1 and *(&Token+1) == "VAR_NAME"))
            {
                std::cout << "Error: Argument name is not set\n";
                exit(-1);
            }

            tempN = new Node;
            tempN->Type = *(&Token+1);
            tempN->Info = *(&Info+1);
            tempN->Mother = MotherNode;
            MotherNode->kind.push_back(tempN);
            MotherNode = MotherNode->Mother;
            ++token_num;
        }

        if ((Token == "VAR_NAME" or Token == "VALUE") and MotherNode->Type == "ARGS") {
            tempN = new Node;
            tempN->Type = Token;
            tempN->Info = Info;
            tempN->Mother = MotherNode;
            MotherNode->kind.push_back(tempN);
        }
    }
}*/

struct MathOrder {
    std::string operationType;
    MathOrder *sub_exp1 = nullptr;
    MathOrder *sub_exp2 = nullptr;
    std::string val1[2] = {"", ""};
    std::string val2[2] = {"", ""};
    MathOrder *Mother   = nullptr;
};

static int findTokenInVector(std::vector<std::string> &vec, std::string element)
{
    for (int num=0; num < vec.size(); ++num) {
        if (vec[num] == element)
            return num;
    }
    return -1;
}

Node* recursionMathParser(std::vector<std::string> expression)
{
    Node *result = new Node;
    for (std::string math_token : {"+", "-", "*", "/"})
    {
        u_int32_t rbrac_count = 0;
        for (u_int32_t token_num=0; token_num < expression.size(); ++token_num) {

            if (expression[token_num] == math_token and rbrac_count == 0) {
                if (expression[token_num] == "/") {
                    for (u_int32_t i = expression.size()-1; i > 0; --i) {
                        if (expression[i] == "/") {
                            token_num = i;
                            break;
                        }
                    }
                }

                result->Type = math_token;
                result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin(), expression.begin()+token_num)));
                result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+token_num+1, expression.end())));
                return result;
            }
            if (expression[token_num] == "(")
                ++rbrac_count;
            if (expression[token_num] == ")")
                --rbrac_count;

            if (expression.size() > 0 and expression[0] == "(" and expression[expression.size()-1] == ")") {
                result->Type = "OP";
                result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+1, expression.end()-1)));
                return result;
            }
            if (expression[token_num] == "") {

            }
        }
    }

    result->Type = "aoe";
    result->Info = expression[0];
    return result;
}

void parseLogicalExpression()
{

}


static void recursionNodePrint(Node *node, uint32_t &k_c)
{
    std::cout << std::string(k_c, '\t') << node->Type << " (" << node->Info << ")" << std::endl;
    ++k_c;
    for (Node *kind : node->kind)
    {
        recursionNodePrint(kind, k_c);
    }
    --k_c;
}

