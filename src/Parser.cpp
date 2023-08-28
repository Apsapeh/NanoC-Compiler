#include "Parser.h"
typedef Parser::Node Node;

static void recursionNodePrint (Node *node, uint32_t &n_c);


Parser::Parser(std::vector<std::vector<Lexer::Token>> &data) {
    std::vector<struct_info> structs;
    std::vector<func_info> functions;
    std::vector<var_info> vars;
    std::vector<var_type> types = {
            {"float", "float"}, {"double", "double"}, {"char", "char"},
            {"short", "short"}, {"int", "int"}, {"long", "long"}
    };

    for (u_int64_t line_num = 0; line_num < data.size(); ++line_num) {
        std::vector<Lexer::Token> &line = data[line_num];
        for (u_int64_t token_num = 0; token_num < line.size(); ++token_num) {
            Lexer::Token &token = line[token_num];


        }
    }
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
Node* Parser::recursionMathParser(std::vector<std::string> expression, std::vector<std::string> expression_info, Node *mother_node, std::string lmore, uint32_t lmore_index)
{

    Node *result = new Node;
    const std::string tokens[] = {"ADD", "MINUS", "MULTIPLE", "DIVIDE", "LLESS", "LMORE", "LEQUAL",
                                  "LLESS_EQ", "LMORE_EQ", "LNOT_EQ", "LAND", "LOR", "LNOT"};
//    for (std::string math_token : {"ADD", "MINUS", "MULTIPLE", "DIVIDE"})
    for (std::string math_token : {"LMORE_EQ", "LLESS_EQ", "LEQUAL", "LNOT_EQ", "ADD", "LOR", "MINUS", "MULTIPLE", "LAND", "DIVIDE", "LMORE", "LLESS", "LNOT"})
    {
        uint32_t rbrac_count = 0;
        for (uint32_t token_num=0; token_num < expression.size(); ++token_num) {
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
                    for (uint32_t i = expression.size()-1; i > 0; --i) {
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
                for (uint32_t i = token_num+1; i < expression.size(); ++i) {
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

