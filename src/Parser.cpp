#include "Parser.h"
typedef Parser::Node Node;

unsigned int findToken(std::vector<std::string> &vec, std::string str);
static void recursionNodePrint (Node *node, uint32_t &n_c);
Node* recursionMathParser(std::vector<std::string> expression, std::vector<std::string> expression_info, Node *mother_node = nullptr,std::string lmore = "", u_int32_t lmore_index = 0);
Node* recursionFuncParser(std::vector<std::string> expression, std::vector<std::string> expression_info , std::string name);


Parser::Parser(std::vector<std::vector<std::string>> TokenizedSource,
               std::vector<std::vector<std::string>> TokensInfo)
{
    Node* Program = new Node; Program->Type = "Program"; Program->Mother = Program;
    Node *MotherNode = Program;

    Node *tempN;
    for (uint32_t line_num=0; line_num < TokenizedSource.size(); ++line_num)
    {
        std::vector<std::string> &VecLine = TokenizedSource[line_num];
        std::vector<std::string> &VecLineInfo = TokensInfo[line_num];
        for (uint32_t token_num=0; token_num < VecLine.size(); ++token_num)
        {
            std::string &Token = VecLine[token_num];
            std::string &Info = VecLineInfo[token_num];

            //std::cout << Token << " ";
            // TODO: Заменить добавление в материнскую ноду на функцию
            if (Token == "FN_DEF" or Token == "FN_CALL" or Token == "IF" or Token == "ELSE" or Token == "WHILE")
            {
                if (Token == "FN_CALL") {
                    int count_of_argsBr = 1;
                    int fn_call_start = token_num+2;
                    for (uint32_t i = token_num + 2; i < VecLine.size(); ++i) {
                        if (VecLine[i] == "ARGS_BEGIN") {
                            //fn_call_start = i + 1;
                            ++count_of_argsBr;
                        } else if (VecLine[i] == "ARGS_END")
                            --count_of_argsBr;

                        if (count_of_argsBr == 0) {
                            MotherNode->kind.push_back(recursionFuncParser(std::vector<std::string>(VecLine.begin() + fn_call_start, VecLine.begin() + i),
                                                                           std::vector<std::string>(VecLineInfo.begin() + fn_call_start, VecLineInfo.begin() + i), VecLineInfo[token_num]));
                            token_num = i;
                            break;
                        }
                    }
                    continue;
                }

                tempN = new Node;
                tempN->Type = Token;
                if (Info != "~~~") {
                    tempN->Info = Info;
                }
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

                //std::cout << Info << std::endl;
            }
            else if (Token == "ARGS_BEGIN" or Token == "COND_BEGIN")
            {
                if (token_num > 0 and *(&Token-1) == "FN_CALL") {
                    continue;
                }

                tempN = new Node;
                tempN->Type = "ARGS";
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);

                if (token_num > 0 and *(&Token-1) == "IF") {
                    int count_of_argsBr = 1;
                    int fn_call_start = token_num+1;
                    for (uint32_t i = token_num + 1; i < VecLine.size(); ++i) {
                        if (VecLine[i] == "COND_BEGIN") {
                            //fn_call_start = i + 1;
                            ++count_of_argsBr;
                        } else if (VecLine[i] == "COND_END") {
                            --count_of_argsBr;
                        }

                        if (count_of_argsBr == 0) {
                            tempN->kind.push_back(recursionMathParser(std::vector<std::string>(VecLine.begin() + fn_call_start, VecLine.begin() + i),
                                                                           std::vector<std::string>(VecLineInfo.begin() + fn_call_start, VecLineInfo.begin() + i)));
                            token_num = i;
                            break;
                        }
                    }
                    continue;
                }
                else
                    MotherNode = tempN;

            }
            else if (Token == "ARGS_END" or Token == "COND_END" or Token == "END") {
                MotherNode = MotherNode->Mother;
            }

            else if (Token == "VAR_TYPE" and MotherNode->Type == "ARGS") {
                tempN = new Node;
                tempN->Type = "ARG";
                MotherNode->kind.push_back(tempN);
                tempN->Mother = MotherNode;
                MotherNode = tempN;

                tempN = new Node;
                tempN->Type = Token;
                tempN->Info = Info;
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);

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

            else if ((Token == "VAR_NAME" or Token == "VALUE") and (MotherNode->Type == "ARGS" or MotherNode->Type == "FN_CALL")) {
                tempN = new Node;
                tempN->Type = Token;
                tempN->Info = Info;
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);
            }

            else if (Token == "VAR_TYPE" and not (token_num+1 < VecLine.size() and *(&Token+1) == "FN_DEF")) {
//                std::cout << *(&Token+2) << std::endl;
                tempN = new Node;
                tempN->Type = "DEFINE_VAR";
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);
                MotherNode = tempN;

                tempN = new Node;
                tempN->Type = Token;
                tempN->Info = Info;
                tempN->Mother = MotherNode;
                MotherNode->kind.push_back(tempN);

                if (token_num+1 < VecLine.size() and *(&Token+1) == "VAR_NAME") {
                    tempN = new Node;
                    tempN->Type = *(&Token+1);
                    tempN->Info = *(&Info+1);
                    tempN->Mother = MotherNode;
                    MotherNode->kind.push_back(tempN);
                }
                else {
                    std::cout << "error\n";
                    exit(1);
                }

                MotherNode = MotherNode->Mother;
                //tempN
            }

            else if (Token == "ASSIGN") {
                tempN = new Node;
                tempN->Type = Token;
                tempN->Mother = MotherNode;

                if (token_num > 0 and *(&Token-1) == "VAR_NAME") {
                    Node *tmp = new Node;
                    tmp->Type = "VAR_NAME";
                    tmp->Info = *(&Info-1);
                    tempN->kind.push_back(tmp);
                } else {
                    std::cout << "error\n";
                    exit(1);
                }
                tempN->kind.push_back(recursionMathParser(std::vector<std::string> (VecLine.begin()+token_num+1, VecLine.end()),
                                                          std::vector<std::string> (VecLineInfo.begin()+token_num+1, VecLineInfo.end())));
                MotherNode->kind.push_back(tempN);
                break;
            }

        }
    }
    ParsedProgram = Program;
    uint32_t kind_counter = 0;
    //recursionNodePrint(Program, kind_counter);
}

Node *Parser::getParsedProgram() {
    return ParsedProgram;
}

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

bool isMathExp(std::vector<std::string> expression)
{
    const std::string tokens[] = {"ADD", "MINUS", "MULTIPLE", "DIVIDE", "LLESS", "LMORE", "LEQUAL",
                                  "LLESS_EQ", "LMORE_EQ", "LNOT_EQ", "LAND", "LOR", "LNOT"};

    for (std::string str : expression) {
        for (std::string tok : tokens) {
            if (str == tok)
                return true;
        }
    }

    return false;
}

Node* recursionFuncParser(std::vector<std::string> expression, std::vector<std::string> expression_info , std::string name)
{
    Node *result = new Node;
    result->Type = "FN_CALL";
    result->Info = name;

    std::vector<std::vector<std::string>> exps;
    std::vector<std::vector<std::string>> exps_info;

    std::vector<std::string> temp_vec;
    std::vector<std::string> temp_vec2;
    for (uint32_t token_num=0; token_num < expression.size(); ++token_num)
    {
        if (expression[token_num] == "SEPARATOR") {
            exps.push_back(temp_vec);
            exps_info.push_back(temp_vec2);
            temp_vec.clear();
            temp_vec2.clear();
        }
        else
        {
            temp_vec.push_back(expression[token_num]);
            temp_vec2.push_back(expression_info[token_num]);
        }
    }
    exps.push_back(temp_vec);
    exps_info.push_back(temp_vec2);

    for (uint32_t token_num=0; token_num < exps.size(); ++token_num)
    {
        std::vector<std::string> &exp = exps[token_num];
        std::vector<std::string> &exp_info = exps_info[token_num];

        if (exp.size() > 0 and exp[0] == "FN_CALL") {

            result->kind.push_back(recursionFuncParser(std::vector<std::string> (exp.begin()+2, exp.end()-1),
                                                       std::vector<std::string> (exp_info.begin()+2, exp_info.end()-1),exp_info[0]));
        }
        else if (isMathExp(exp)) {
            result->kind.push_back(recursionMathParser(exp, exp_info));
        }
        else if (exp.size() > 0)
        {
            Node *tempN = new Node;
            tempN->Type = exp[0];
            if (expression_info[0] != "~~~")
                tempN->Info = expression_info[0];

            result->kind.push_back(tempN);
        }
    }

    return result;
}

// Алгоритм парсинга мат. выражений был придумал и написан Евгением Вениковым на Python, я его переписал на C++ и доработал
Node* recursionMathParser(std::vector<std::string> expression, std::vector<std::string> expression_info, Node *mother_node, std::string lmore, u_int32_t lmore_index)
{

    Node *result = new Node;
    const std::string tokens[] = {"ADD", "MINUS", "MULTIPLE", "DIVIDE", "LLESS", "LMORE", "LEQUAL",
                                  "LLESS_EQ", "LMORE_EQ", "LNOT_EQ", "LAND", "LOR", "LNOT"};
//    for (std::string math_token : {"ADD", "MINUS", "MULTIPLE", "DIVIDE"})
    for (std::string math_token : {"LMORE_EQ", "LLESS_EQ", "LEQUAL", "LNOT_EQ", "ADD", "LOR", "MINUS", "MULTIPLE", "LAND", "DIVIDE", "LMORE", "LLESS", "LNOT"})
    {
        u_int32_t rbrac_count = 0;
        for (u_int32_t token_num=0; token_num < expression.size(); ++token_num) {

            if (lmore != "") {
                result->Type = lmore;
                result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin(), expression.begin()+lmore_index),
                                                           std::vector<std::string>(expression_info.begin(), expression_info.begin()+lmore_index), result));
                result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+lmore_index+1, expression.end()),
                                                           std::vector<std::string>(expression_info.begin()+lmore_index+1, expression_info.end()), result));
                return result;
            }

            if (expression[token_num] == math_token and rbrac_count == 0) {
                if (expression[token_num] == "DIVIDE") {
                    for (u_int32_t i = expression.size()-1; i > 0; --i) {
                        if (expression[i] == "DIVIDE") {
                            token_num = i;
                            break;
                        }
                    }
                }

                if (expression[token_num] == "LNOT") {
                    result->Type = math_token;
                    result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+token_num+1, expression.end()),
                                                               std::vector<std::string>(expression_info.begin()+token_num+1, expression_info.end()), result));
                    return result;
                }

                if (expression[token_num] == "" or expression[token_num] == "") {
                    result->Type = "LOR";
                    result->kind.push_back(recursionMathParser(expression, expression_info, result, (std::string) expression[token_num], token_num));
                    result->kind.push_back(recursionMathParser(expression, expression_info, result, "LEQUAL", token_num));
                    return result;
                }

                result->Type = math_token;
                result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin(), expression.begin()+token_num),
                                                           std::vector<std::string>(expression_info.begin(), expression_info.begin()+token_num), result));
                result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+token_num+1, expression.end()),
                                                           std::vector<std::string>(expression_info.begin()+token_num+1, expression_info.end()), result));
                return result;
            }
            if (expression[token_num] == "RBRAC_BEGIN")
                ++rbrac_count;
            if (expression[token_num] == "RBRAC_END")
                --rbrac_count;

            if (expression.size() > 0 and expression[0] == "RBRAC_BEGIN" and expression[expression.size()-1] == "RBRAC_END") {

                if (mother_node == nullptr) {
                    result = recursionMathParser(std::vector<std::string>(expression.begin()+1, expression.end()-1),
                                                 std::vector<std::string>(expression_info.begin()+1, expression_info.end()-1));
                    return result;
                }
                else {
                    mother_node->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+1, expression.end()-1),
                                                                    std::vector<std::string>(expression_info.begin()+1, expression_info.end()-1)));
                    return nullptr;
                }

            }
            if (expression[token_num] == "FN_CALL") {
                int count_of_argsBr = 0;
                int fn_call_start   = token_num+2;
                for (uint32_t i = token_num+1; i < expression.size(); ++i) {
                    if (expression[i] == "ARGS_BEGIN") {
                        //fn_call_start = i + 1;
                        ++count_of_argsBr;
                    }
                    else if (expression[i] == "ARGS_END")
                        --count_of_argsBr;

                    if (count_of_argsBr == 0) {
                        return recursionFuncParser(std::vector<std::string>(expression.begin()+fn_call_start, expression.begin()+i),
                                                   std::vector<std::string>(expression_info.begin()+fn_call_start, expression_info.begin()+i), expression_info[token_num]);
                    }
                }
            }
        }
    }

    result->Type = expression[0];
    if (expression_info[0] != "~~~")
        result->Info = expression_info[0];
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
        if (kind != nullptr)
            recursionNodePrint(kind, k_c);
    }
    --k_c;
}

