#include <iostream>
#include <vector>
#include <algorithm>
#include "Error.h"
#include "PreProcessor.h"

std::vector<std::string> PreProcessor::req(std::vector<std::string> &line, uint32_t start_pose, std::vector<DefineStruct> *defines) {
    std::vector<std::string> result;
    for (u_int32_t word_index = start_pose; word_index < line.size(); ++word_index) {
        std::string word = line[word_index];
        int64_t defineIndex = getDefineIndex(word, defines);

        if (defineIndex != -1) {
            if (word_index < line.size() and line[word_index+1] == "(") {
                word_index += 2;
                uint32_t slice_size = 2;
                u_int32_t rb_br_counter = 1;
                while (word_index < line.size()) {
                    if (line[word_index] == "(")
                        ++rb_br_counter;
                    else if (line[word_index] == ")")
                        --rb_br_counter;

                    ++word_index;
                    ++slice_size;

                    if (rb_br_counter == 0)
                        break;

                }
                std::vector<std::string> tmp_vec(line.begin()+word_index-slice_size, line.begin()+word_index);
                req(tmp_vec, 0, defines);
            }
        }
        else {
            result.push_back(word);
        }
    }

    return result;
}

PreProcessor::PreProcessor(std::string source) {
    std::vector<std::string> separated_source;

    // Деление строки по новой стркоке
    std::string tmpStr = "";
    char prevCh = ' ';
    bool is_string = false;
    bool is_comment = false;
    u_int32_t multiline_comment_counter = 0;

    for (u_int32_t char_counter = 0; char_counter < source.size(); ++char_counter) {
        char &ch = source[char_counter];

        if (ch == '"')
            is_string = not is_string;

        if (ch == '/' and char_counter < source.size()-1 and *(&ch+1) == '*' and not is_string) {
            ++multiline_comment_counter;
            ++char_counter;
        }
        if (ch == '/' and char_counter <= source.size() and *(&ch+1) == '/' and not is_string)
            is_comment = true;


        if (not is_comment and multiline_comment_counter == 0) {
            if ((ch == '\n' or ch == '\0') and prevCh != '\\') {
                if (not tmpStr.empty() and tmpStr != "\n") {
                    separated_source.push_back(tmpStr);
                    tmpStr.clear();
                }
            }
            else if (ch != '\n' and ch != '\0' and ch != '\\')
                tmpStr.push_back(ch);

        }

        if (ch == '*' and char_counter < source.size()-1 and *(&ch+1) == '/' and not is_string) {
            --multiline_comment_counter;
            ++char_counter;
        }
        if (ch == '\n' and is_comment)
            is_comment = false;

        prevCh = ch;
    }
    if (not tmpStr.empty())
        separated_source.push_back(tmpStr);


    // Делим строку на слова
    std::vector<std::vector<std::string>> separated_line;
    for (std::string str : separated_source) {
        bool is_string = false;
        std::vector<std::string> tmpVec;
        tmpStr.clear();
        for (char &ch : str) {
            if (ch == '"')
                is_string = not is_string;

            if ((ch == ' ' or ch == '(' or ch == ')' or ch == ';' or ch == ',' /*or ch == '{' or ch == '}'*/) and not is_string) {

                if (not tmpStr.empty()) {
                    tmpVec.push_back(tmpStr);
                    tmpStr.clear();
                }

                if (tmpVec.size() == 2 and tmpVec[0] == "#define" and ch == '(' and *(&ch -1) != ' ') {
                    tmpVec.push_back("_");
                }

                if (ch == '(' or ch == ')' or ch == ';' or ch == ',') {
                    tmpStr.push_back(ch);
                    tmpVec.push_back(tmpStr);
                    tmpStr.clear();
                }
            }
            else
                tmpStr.push_back(ch);
        }
        if (not tmpStr.empty())
            tmpVec.push_back(tmpStr);

        if (not tmpVec.empty())
            separated_line.push_back(tmpVec);
    }
    separated_source = std::vector<std::string>();


    std::vector<std::string> resultVec;
    resultVec.reserve(separated_line.size() * 30);
    std::vector<DefineStruct> defines;
    for (std::vector<std::string> &line : separated_line) {
        if (line[0] == "#define") {
            std::string name = line[1];
            std::vector<std::string> args, body;
            bool is_args = false;
            for (uint32_t wordIndex = 2; wordIndex < line.size(); ++wordIndex) {
                std::string &word = line[wordIndex];
                if (word == "_") {
                    is_args = true;
                    ++wordIndex;
                    continue;
                }

                if (is_args) {
                    if (word != "," and word != ")")
                        args.push_back(word);
                    else if (word == ")")
                        is_args = false;
                }
                else
                    body.push_back(word);
            }
            int64_t defIndex = getDefineIndex(name, &defines);
            if (defIndex == -1)
                defines.push_back({name, args, body});
            else {
                defines[defIndex].Args = args;
                defines[defIndex].Body = body;
            }
        }
        else if (line[0] == "#undef") {
            if (line.size() < 2)
                Error::throwError("You did not specify a macro name");
            else if (line.size() > 2)
                Error::throwError("Too many parameters are specified for the #undef directive");

            int64_t def_index = getDefineIndex(line[1], &defines);
            if (def_index == -1)
                Error::throwError("The \""+line[1]+"\" macro does not exist");

            defines.erase(defines.begin() + def_index);
        }
        else {
            for (uint32_t wordIndex = 0; wordIndex < line.size(); ++wordIndex) {
                std::string &word = line[wordIndex];
                int64_t defIndex = getDefineIndex(word, &defines);

                if (defIndex == -1)
                    resultVec.push_back(word);
                else {
                    if (wordIndex+1 < line.size() and line[wordIndex+1] == "(") {
                        wordIndex += 2;
                        uint32_t rb_br_counter = 1;
                        std::vector<std::string> tmpVec;
                        while (wordIndex < line.size()) {
                            if (line[wordIndex] == "(")
                                ++rb_br_counter;
                            else if (line[wordIndex] == ")")
                                --rb_br_counter;
                            ++wordIndex;

                            if (rb_br_counter == 0)
                                break;

                            tmpVec.push_back(line[wordIndex - 1]);
                        }
                        --wordIndex;

                        std::vector<std::string> res = changeDefineToString(defines[defIndex], tmpVec);
                        resultVec.insert(resultVec.end(), res.begin(), res.end());
                    }
                    else {
                        std::vector<std::string> tmp_vec;
                        std::vector<std::string> res = changeDefineToString(defines[defIndex], tmp_vec);
                        resultVec.insert(resultVec.end(), res.begin(), res.end());
                    }
                }
            }
        }
    }
    separated_line = std::vector<std::vector<std::string>>();

    resultVec.shrink_to_fit();
    for (std::string str : resultVec) {
        std::cout << str << " ";
    }

}

void PreProcessor::AddDefine(
        std::string &word, std::vector<DefineStruct> &defines,  u_int32_t &define_index, u_int32_t &local_rbrac_counter,
        std::vector<std::string> &tmp_vec_for_args, std::vector<std::string> &vec_to_push_result
) {
    if (define_index == -1) {
        define_index = getDefineIndex(word, &defines);
        local_rbrac_counter = 0;
    }
    if (define_index != -1) {
        if (defines[define_index].Args.empty()) {
            vec_to_push_result.insert(vec_to_push_result.end(), defines[define_index].Body.begin(), defines[define_index].Body.end());
        }
        else {
            if (word == "(") {
                ++local_rbrac_counter;
            };
            if (word == ")") {
                --local_rbrac_counter;
            };

            if (local_rbrac_counter == 0) {
                if (not tmp_vec_for_args.empty()) {
                    changeDefineToString(defines[define_index], tmp_vec_for_args);
                    define_index = -1;
                }
            }
            else {
                tmp_vec_for_args.push_back(word);
            }
        }
    }
    else {
        vec_to_push_result.push_back(word);
    }
}

std::vector<std::string> PreProcessor::changeDefineToString(PreProcessor::DefineStruct def_data, std::vector<std::string> &args_line) {

    std::vector<std::vector<std::string>> args;
    uint32_t rb_counter = 0;
    std::vector<std::string> tmp_arg;
    for (uint32_t word_index = 0; word_index < args_line.size(); ++word_index) {
        std::string &word = args_line[word_index];

        if (rb_counter == 0 and word == ",") {
            args.push_back(tmp_arg);
            tmp_arg.clear();
            continue;
        }

        if (word == "(")
            ++rb_counter;
        if (word == ")")
            --rb_counter;

        tmp_arg.push_back(word);
    }

    if (not tmp_arg.empty()) {
        args.push_back(tmp_arg);
        tmp_arg.clear();
    }

    if (args.size() != def_data.Args.size()) {
        Error::throwError("Found "+std::to_string(args.size())+" arguments, but "+std::to_string(def_data.Args.size())+" are required");
    }

    std::vector<std::string> result;
    for (std::string &word : def_data.Body) {
        std::vector<std::string>::iterator arg_iter =  std::find(def_data.Args.begin(), def_data.Args.end(), word);


        if (arg_iter != def_data.Args.end()) {
            result.insert(result.end(), args[arg_iter-def_data.Args.begin()].begin(), args[arg_iter-def_data.Args.begin()].end());
        }
        else
            result.push_back(word);
    }


    return result;
}

int64_t PreProcessor::getDefineIndex(std::string str, std::vector<DefineStruct> *def_vec) {
    for (int64_t n = 0; n < def_vec->size(); ++n) {
        if ((*def_vec)[n].Name == str)
            return (int64_t)n;
    }
    return (int64_t)-1;
}