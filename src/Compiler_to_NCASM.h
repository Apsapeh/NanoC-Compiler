#pragma once

#include "Parser.h"
#include <vector>
#include <map>
#include <cstdint>


class Compiler_to_NCASM {
public:
    enum nodeType {
        PROGRAM,
        DEF_VAR,
        ASSIGN
    };

    struct NCASM_Instruction {
        std::string OPcode;
        std::vector<std::string> Args;
    };

    Compiler_to_NCASM(Parser::Node* node);
    std::vector<NCASM_Instruction>* getCompiledCode();
private:
    std::map<std::string, uint64_t> variablesBinds;
    std::vector<NCASM_Instruction> CompiledCode;
    void recursionNodeParse(Parser::Node *node, uint64_t &k_c,
                                   nodeType type_of_parrent, NCASM_Instruction *temp_instr = nullptr);
    void recursionMathExpCompiler(Parser::Node *node, int64_t num, bool is_logical = false);

    void addInstrToCompiledCode(std::string OPCode, std::string arg1 = "", std::string arg2 = "", std::string arg3 = "");
};