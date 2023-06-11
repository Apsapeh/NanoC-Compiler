#include "Parser.h"
typedef Parser::Node Node;

static void recursionNodePrint (Node *node, u_int32_t &n_c);


Parser::Parser(std::vector<std::vector<std::string>> TokenizedSource,
               std::vector<std::vector<std::string>> TokensInfo)
{
    Node* Program = new Node; Program->Type = "Program"; Program->Mother = Program;
    Node *MotherNode = Program;

    //Node *tempN;
    std::string tmp_string = "";
    for (u_int32_t line_num=0; line_num < TokenizedSource.size(); ++line_num)
    {
        std::vector<std::string> &VecLine = TokenizedSource[line_num];
        std::vector<std::string> &VecLineInfo = TokensInfo[line_num];
        for (u_int32_t token_num=0; token_num < VecLine.size(); ++token_num)
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
                    for (u_int32_t i = token_num + 2; i < VecLine.size(); ++i) {
                        if (VecLine[i] == "ARGS_BEGIN") {
                            //fn_call_start = i + 1;
                            ++count_of_argsBr;
                        } else if (VecLine[i] == "ARGS_END")
                            --count_of_argsBr;

                        if (count_of_argsBr == 0) {
                            MotherNode->kind.push_back(recursionFuncParser(
                                    std::vector<std::string>(VecLine.begin() + fn_call_start, VecLine.begin() + i),
                                    std::vector<std::string>(VecLineInfo.begin() + fn_call_start, VecLineInfo.begin() + i), VecLineInfo[token_num])
                            );
                            token_num = i;
                            break;
                        }
                    }
                    continue;
                }

                if (Info != "~~~")
                    tmp_string = Info;
                else
                    tmp_string = "";

                MotherNode = addNode(Token, tmp_string, MotherNode);

                if (Token == "FN_DEF" and token_num > 0 and *(&Token-1) == "VAR_TYPE")
                    addNode("RETURNING_TYPE", *(&Info-1), MotherNode);

                else if (Token == "FN_DEF" and not (token_num > 0 and *(&Token-1) == "VAR_TYPE")) {
                    std::cout << "Error: The return type of the function is undefined\n";
                    exit(-1);
                }
                //std::cout << Info << std::endl;
            }
            else if (Token == "ARGS_BEGIN" or Token == "COND_BEGIN")
            {
                if (token_num > 0 and *(&Token-1) == "FN_CALL")
                    continue;

                Node* tempN = addNode("ARGS", "", MotherNode);

                if (token_num > 0 and (*(&Token-1) == "IF" or *(&Token-1) == "WHILE")) {
                    int count_of_argsBr = 1;
                    int fn_call_start = token_num+1;
                    for (u_int32_t i = token_num + 1; i < VecLine.size(); ++i) {
                        if (VecLine[i] == "COND_BEGIN")
                            ++count_of_argsBr;
                        else if (VecLine[i] == "COND_END")
                            --count_of_argsBr;

                        if (count_of_argsBr == 0) {
                            tempN->kind.push_back(recursionMathParser(
                                std::vector<std::string>(VecLine.begin() + fn_call_start, VecLine.begin() + i),
                                std::vector<std::string>(VecLineInfo.begin() + fn_call_start, VecLineInfo.begin() + i), MotherNode)
                            );
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
                MotherNode = addNode("ARG", "", MotherNode);
                addNode(Token, Info, MotherNode);

                if (not (token_num < VecLine.size() - 1 and *(&Token+1) == "VAR_NAME")) {
                    std::cout << "Error: Argument name is not set\n";
                    exit(-1);
                }

                addNode(*(&Token+1), *(&Info+1), MotherNode);
                MotherNode = MotherNode->Mother;
                ++token_num;
            }

            else if ((Token == "VAR_NAME" or Token == "VALUE") and (MotherNode->Type == "ARGS" or MotherNode->Type == "FN_CALL"))
                addNode(Token, Info, MotherNode);


            else if (Token == "VAR_TYPE" and not (token_num+1 < VecLine.size() and *(&Token+1) == "FN_DEF")) {
                MotherNode = addNode("DEFINE_VAR", "", MotherNode);
                addNode(Token, Info, MotherNode);

                if (token_num+1 < VecLine.size() and *(&Token+1) == "VAR_NAME")
                    addNode(*(&Token+1), *(&Info+1), MotherNode);

                else {
                    std::cout << "error\n";
                    exit(1);
                }

                MotherNode = MotherNode->Mother;
            }

            else if (Token == "ASSIGN") {
                Node* tempN = addNode(Token, "", MotherNode);

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
                                                          std::vector<std::string> (VecLineInfo.begin()+token_num+1, VecLineInfo.end()), MotherNode));

                break;
            }

            else if (Token == "BREAK")
                addNode(Token, "", MotherNode);
        }
    }
    ParsedProgram = Program;
    u_int32_t kind_counter = 0;
    recursionNodePrint(Program, kind_counter);
}

Node *Parser::getParsedProgram() {
    return ParsedProgram;
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

Node* Parser::addNode(std::string type, std::string info, Node* mother_node, bool add_to_mother) {
    Node* tmp_node = new Node;
    tmp_node->Type = type;
    tmp_node->Info = info;

    if (mother_node != nullptr) {
        tmp_node->Mother = mother_node;
        if (add_to_mother)
            mother_node->kind.push_back(tmp_node);
    }

    return tmp_node;
}

Node* Parser::recursionFuncParser(std::vector<std::string> expression, std::vector<std::string> expression_info , std::string name)
{
    Node *result = new Node;
    result->Type = "FN_CALL";
    result->Info = name;

    std::vector<std::vector<std::string>> exps;
    std::vector<std::vector<std::string>> exps_info;

    std::vector<std::string> temp_vec;
    std::vector<std::string> temp_vec2;
    for (u_int32_t token_num=0; token_num < expression.size(); ++token_num)
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

    for (u_int32_t token_num=0; token_num < exps.size(); ++token_num)
    {
        std::vector<std::string> &exp = exps[token_num];
        std::vector<std::string> &exp_info = exps_info[token_num];

        if (exp.size() > 0 and exp[0] == "FN_CALL") {

            result->kind.push_back(recursionFuncParser(std::vector<std::string> (exp.begin()+2, exp.end()-1),
                                                       std::vector<std::string> (exp_info.begin()+2, exp_info.end()-1),exp_info[0]));
        }
        else if (isMathExp(exp)) {
            result->kind.push_back(recursionMathParser(exp, exp_info, nullptr));
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

// FIXME: Парсер плохо работает
// Алгоритм парсинга мат. выражений был придумал и написан Евгением Вениковым на Python, я его переписал на C++ и доработал
Node* Parser::recursionMathParser(std::vector<std::string> expression, std::vector<std::string> expression_info, Node *mother_node, std::string lmore, u_int32_t lmore_index)
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
                result->Mother = mother_node;
                result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin(), expression.begin()+lmore_index),
                                                           std::vector<std::string>(expression_info.begin(), expression_info.begin()+lmore_index), result));
                result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+lmore_index+1, expression.end()),
                                                           std::vector<std::string>(expression_info.begin()+lmore_index+1, expression_info.end()), result));
                return result;
            }

            if (expression[token_num] == math_token and rbrac_count == 0) {
                result->Mother = mother_node;
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
                    result->Mother = mother_node;
                    result->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+token_num+1, expression.end()),
                                                               std::vector<std::string>(expression_info.begin()+token_num+1, expression_info.end()), result));
                    return result;
                }

                if (expression[token_num] == "" or expression[token_num] == "") {
                    result->Type = "LOR";
                    result->Mother = mother_node;
                    result->kind.push_back(recursionMathParser(expression, expression_info, result, (std::string) expression[token_num], token_num));
                    result->kind.push_back(recursionMathParser(expression, expression_info, result, "LEQUAL", token_num));
                    return result;
                }

                result->Type = math_token;
                result->Mother = mother_node;
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
                                                 std::vector<std::string>(expression_info.begin()+1, expression_info.end()-1), result);
                    result->Mother = mother_node;
                    return result;
                }
                else {
                    mother_node->kind.push_back(recursionMathParser(std::vector<std::string>(expression.begin()+1, expression.end()-1),
                                                                    std::vector<std::string>(expression_info.begin()+1, expression_info.end()-1), mother_node));
                    return nullptr;
                }

            }
            if (expression[token_num] == "FN_CALL") {
                int count_of_argsBr = 0;
                int fn_call_start   = token_num+2;
                for (u_int32_t i = token_num+1; i < expression.size(); ++i) {
                    if (expression[i] == "ARGS_BEGIN")
                        ++count_of_argsBr;

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
    result->Mother = mother_node;
    if (expression_info[0] != "~~~")
        result->Info = expression_info[0];
    return result;
}


static void recursionNodePrint(Node *node, u_int32_t &k_c)
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

