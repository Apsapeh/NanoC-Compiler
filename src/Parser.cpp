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
        }
    }
    uint32_t kind_counter = 0;
    recursionNodePrint(&Program, kind_counter);
    std::cout << "\n\n\n\n";
    Node* a = recursionMathParser(std::vector<std::string> {"1", "+", "2", "*", "3", "/", "(", "1", "+", "1", ")"});
    kind_counter = 0;
    recursionNodePrint(a, kind_counter);
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
    Node result;
    for (std::string math_token : {"+", "-", "*", "/"})
    {
        u_int32_t rbrac_count = 0;
        for (u_int32_t token_num=0; token_num < expression.size(); ++token_num) {
            if (expression[token_num] == math_token and rbrac_count == 0) {
                result.Type = "OP";
                result.kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin(), expression.begin()+token_num)));
                result.kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+token_num+1, expression.end())));
                return &result;
            }
            if (expression[token_num] == "(")
                ++rbrac_count;
            if (expression[token_num] == ")")
                --rbrac_count;

            if (expression.size() > 0 and expression[0] == "(" and expression[expression.size()-1] == ")") {
                //result.Type = "OP";
                result.kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+1, expression.end()-1)));
                return &result;
            }
            if (expression[token_num] == "") {

            }
        }
    }

    result.Type = "aoe";
    result.Info = expression[0];
    return &result;
}

Node* parseMathExpression(std::vector<std::string> expression)
{
    Node *MotherNode = new Node;
    Node *TempNode;

    MathOrder *math_order;
    math_order->Mother = math_order;

    uint32_t start_pose = 0;
    while (expression.size() != 0) {
        for (uint32_t tok_num = start_pose; tok_num < expression.size(); ++tok_num) {
            if (expression[tok_num] == "RBRAC_BEGIN") {
                start_pose = tok_num + 1;
                break;
            }
        }



    }
}

static MathOrder recursionRightMathOrder(std::vector<std::string> &expression)
{
    MathOrder result;
    bool result_is_clear = true;
    int token_pos = -1;
    token_pos = findTokenInVector(expression, "RBRAC_BEGIN");
    if (token_pos != -1) {
        if (result_is_clear) {
            //result =
        }
    }
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

