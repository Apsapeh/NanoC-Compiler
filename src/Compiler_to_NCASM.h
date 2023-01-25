#pragma once

#include "Parser.h"
#include <vector>

class Compiler_to_NCASM {
public:
    enum nodeType {
        PROGRAM,
        DEF_VAR
    };

    struct NCASM_Instruction {
        std::string OPcode;
        std::vector<std::string> Args;
    };

    Compiler_to_NCASM(Parser::Node* node);
private:
    std::vector<NCASM_Instruction> CompiledCode;
    void recursionNodeParse(Parser::Node *node, uint64_t &k_c,
                                   nodeType type_of_parrent, NCASM_Instruction *temp_instr = nullptr);
};