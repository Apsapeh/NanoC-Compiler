#include <algorithm>
#include "main.h"
#include "Error.h"
#include "Lexer.h"

static void add_info_to_temp_vectors(std::vector<std::string> &tok_vec, std::vector<std::string> &tok_info_vec, std::string token, std::string token_info);

Lexer::Lexer(std::vector<std::vector<std::string>> processed_source)
{
    for (uint64_t line_num = 0; line_num < processed_source.size(); ++line_num) {
        std::vector<std::string> &line = processed_source[line_num];
        std::vector<Token> line_tokens;
        for (uint64_t word_num = 0; word_num < line.size(); ++word_num) {
            std::string &word = line[word_num];
            std::cout << word << " | ";
            if (word == "(")
                line_tokens.push_back({TokenType::RBR_BEG, ""});
            else if (word == ")")
                line_tokens.push_back({TokenType::RBR_END, ""});
            else if (word == "[")
                line_tokens.push_back({TokenType::SBR_BEG, ""});
            else if (word == "]")
                line_tokens.push_back({TokenType::SBR_END, ""});
            else if (word == "{")
                line_tokens.push_back({TokenType::BEGIN, ""});
            else if (word == "}")
                line_tokens.push_back({TokenType::END, ""});
            else if (word == ",")
                line_tokens.push_back({TokenType::SEPARATOR, ""});
            else if (word == ".")
                line_tokens.push_back({TokenType::DOT, ""});
            else if (word == "=")
                line_tokens.push_back({TokenType::ASSIGN, ""});
            else if (word == "+")
                line_tokens.push_back({TokenType::ADD, ""});
            else if (word == "-")
                line_tokens.push_back({TokenType::MINUS, ""});
            else if (word == "*")
                line_tokens.push_back({TokenType::MULT, ""});
            else if (word == "/")
                line_tokens.push_back({TokenType::DIV, ""});
            else if (word == "%")
                line_tokens.push_back({TokenType::MOD, ""});
            else if (word == "+=")
                line_tokens.push_back({TokenType::ASSIGN_ADD, ""});
            else if (word == "-=")
                line_tokens.push_back({TokenType::ASSIGN_MINUS, ""});
            else if (word == "*=")
                line_tokens.push_back({TokenType::ASSIGN_MULT, ""});
            else if (word == "/=")
                line_tokens.push_back({TokenType::ASSIGN_DIV, ""});
            else if (word == "%=")
                line_tokens.push_back({TokenType::ASSIGN_MOD, ""});
            else if (word == "<")
                line_tokens.push_back({TokenType::LL, ""});
            else if (word == ">")
                line_tokens.push_back({TokenType::LM, ""});
            else if (word == "<=")
                line_tokens.push_back({TokenType::LLQ, ""});
            else if (word == ">=")
                line_tokens.push_back({TokenType::LMQ, ""});
            else if (word == "==")
                line_tokens.push_back({TokenType::LQ, ""});
            else if (word == "&&" or word == "and")
                line_tokens.push_back({TokenType::LAND, ""});
            else if (word == "||" or word == "or")
                line_tokens.push_back({TokenType::LOR, ""});
            else if (word == "!" or word == "not")
                line_tokens.push_back({TokenType::LNOT, ""});
            else if (word == "if")
                line_tokens.push_back({TokenType::IF, ""});
            else if (word == "else")
                line_tokens.push_back({TokenType::ELSE, ""});
            else if (word == "while")
                line_tokens.push_back({TokenType::WHILE, ""});
            else if (word == "struct")
                line_tokens.push_back({TokenType::STRUCT, ""});
            else if (isNumber(word) and word_num+1 < line.size() and line[word_num+1] == "."
                    and word_num+2 < line.size() and isNumber(line[word_num+2])) {
                line_tokens.push_back({TokenType::VALUE, word + "." + line[word_num+2]});
                word_num += 2;
            }
            else
                line_tokens.push_back({TokenType::VALUE, word});

            /*else if (word == "")
                line_tokens.push_back({TokenType::, ""});
            */
        }
        this->TokenizedSource.push_back(line_tokens);
        std::cout << "\n";
    }

}

bool Lexer::isNumber(std::string &str) {
    for (char ascii_num : str)
        if (not (ascii_num >= '0' and ascii_num <= '9'))
            return false;
    return true;
}

std::vector<std::vector<Lexer::Token>>& Lexer::getLexedSource() {
    return this -> TokenizedSource;
}