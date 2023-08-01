#pragma once

#include <string>
#include "Compiler_NCASM_to_ByteCode.h"

class PreProcessor {
public:
    PreProcessor(std::string source);

    std::vector<std::string> getProcessedSourceVec();
    std::string              getProcessedSourceStr();
    std::vector<std::vector<std::string>> getProcessedSourceVecLines();

private:
    std::vector<std::string> ProcessedSource;
    struct DefineStruct {
        std::string Name;
        std::vector<std::string> Args;
        std::vector<std::string> Body;
    };

    std::vector<std::string> parseDefineString(std::vector<std::string> &line, uint32_t &wordIndex, DefineStruct defineData);
    std::vector<std::string> changeDefineToString(DefineStruct def_data, std::vector<std::string> &args_line);
    int64_t getDefineIndex(std::string str, std::vector<DefineStruct> *def_vec);
};

