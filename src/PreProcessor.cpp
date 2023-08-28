#include <iostream>
#include <vector>
#include <algorithm>
#include "Error.h"
#include "PreProcessor.h"


PreProcessor::PreProcessor(std::string source) {
    std::vector<std::string> separated_source;

    // Деление строки по новой строке
    std::string tmpStr = "";
    char prevCh = ' ';
    bool is_string = false;
    bool is_comment = false;
    uint32_t multiline_comment_counter = 0;
    for (uint32_t char_counter = 0; char_counter < source.size(); ++char_counter) {
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
    char sep_by_ch[] = {
            '(', ')', '[', ']', ';', ',', '.',
            '{', '}', '=', '+', '-', '*',
            '/', '%'
    };
    std::vector<std::vector<std::string>> separated_line;
    for (std::string str : separated_source) {
        bool is_string = false;
        std::vector<std::string> tmpVec;
        tmpStr.clear();
        for (uint64_t i = 0; i < str.size(); ++i) {
            char &ch = str[i];
            if (ch == '"')
                is_string = not is_string;

            if ((ch == ' ' or std::find(std::begin(sep_by_ch), std::end(sep_by_ch), ch) != std::end(sep_by_ch)) and not is_string) {
                if (not tmpStr.empty() and tmpStr != "unsigned") {
                    tmpVec.push_back(tmpStr);
                    tmpStr.clear();
                }
                else if (tmpStr == "unsigned")
                    tmpStr.push_back(' ');

                if (tmpVec.size() == 2 and tmpVec[0] == "#define" and ch == '(' and *(&ch -1) != ' ') {
                    tmpVec.push_back("_");
                }

                if (std::find(std::begin(sep_by_ch), std::end(sep_by_ch), ch) != std::end(sep_by_ch)) {
                    tmpStr.push_back(ch);
                    if (i+1 < str.size() and str[i+1] == '=' and
                        (ch == '+' or ch == '-' or ch == '*' or ch == '/' or ch == '%' or ch == '=')
                    ) {
                        tmpStr.push_back('=');
                        ++i;
                    }
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


    std::vector<DefineStruct> defines = {
            {"true" , {}, {"1"}},
            {"false", {}, {"0"}},
    };
    std::vector<std::string> resultVec;
    resultVec.reserve(separated_line.size() * 30);
    for (uint64_t lineIndex = 0; lineIndex < separated_line.size(); ++lineIndex) {
        std::vector<std::string> &line = separated_line[lineIndex];
        if (line[0] == "#include") {
            // TODO: Add #include
        }
        else if (line[0] == "#define") {
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
                else {
                    int64_t defIndex = getDefineIndex(word, &defines);
                    if (defIndex != -1) {
                        std::vector<std::string> res = parseDefineString(line, wordIndex, defines[defIndex]);
                        body.insert(body.end(), res.begin(), res.end());
                    }
                    else
                        body.push_back(word);
                }
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

        else if (line[0] == "#ifdef" or line[0] == "#ifndef") {
            if (line.size() < 2)
                Error::throwError("You did not specify a macro name");
            else if (line.size() > 2)
                Error::throwError("Too many parameters are specified for the #undef directive");

            int64_t def_index = getDefineIndex(line[1], &defines);
            if ((line[0] == "#ifdef" and def_index == -1) or (line[0] == "#ifndef" and def_index != -1)) {
                while (lineIndex < separated_line.size()) {
                    if (separated_line[lineIndex][0] == "#endif")
                        break;
                    else
                        ++lineIndex;
                }
            }
        }

        else if (line[0] == "#endif")
            continue;

        else {
            for (uint32_t wordIndex = 0; wordIndex < line.size(); ++wordIndex) {
                std::string &word = line[wordIndex];
                int64_t defIndex = getDefineIndex(word, &defines);

                if (defIndex == -1)
                    resultVec.push_back(word);
                else {
                    std::vector<std::string> res = parseDefineString(line, wordIndex, defines[defIndex]);
                    resultVec.insert(resultVec.end(), res.begin(), res.end());
                }
            }
        }
    }

    resultVec.shrink_to_fit();

    for (std::string str : resultVec) {
        std::cout << str << " ";
    }
    std::cout << "\n";
    ProcessedSource = resultVec;
}


std::vector<std::string> PreProcessor::parseDefineString(std::vector<std::string> &line, uint32_t &wordIndex, DefineStruct defineData) {
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

        return changeDefineToString(defineData, tmpVec);
    }
    else {
        std::vector<std::string> tmp_vec;
        return changeDefineToString(defineData, tmp_vec);
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



//////  PUBLIC:

std::vector<std::string> PreProcessor::getProcessedSourceVec() {
    return ProcessedSource;
}

std::string PreProcessor::getProcessedSourceStr() {
    std::string result;
    result.reserve(ProcessedSource.size() * 5);
    for (std::string &str : ProcessedSource)
        result += str + " ";
    return result;
}

std::vector<std::vector<std::string>> PreProcessor::getProcessedSourceVecLines() {
    std::vector <std::vector<std::string>> result;
    std::vector<std::string> tmp_vec;
    for (std::string &word : ProcessedSource) {
        if (word == ";") {
            if (not tmp_vec.empty()) {
                result.push_back(tmp_vec);
                tmp_vec.clear();
            }
        }
        else if (word == "{" or word == "}") {
            if (not tmp_vec.empty()) {
            //    tmp_vec.push_back(word);
                result.push_back(tmp_vec);
                tmp_vec.clear();
            }
            result.push_back({word});
        }
        else {
            tmp_vec.push_back(word);
        }
    }

    if (not tmp_vec.empty())
        result.push_back(tmp_vec);


    for (auto a : result) {
        for (auto b : a) {
            //std::cout << b << " ";
        }
        //std::cout << "\n";
    }

    return result;
}



