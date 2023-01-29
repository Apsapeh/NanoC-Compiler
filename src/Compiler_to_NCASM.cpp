#include "Compiler_to_NCASM.h"
#include <algorithm>

Compiler_to_NCASM::Compiler_to_NCASM(Parser::Node* node)
{

    uint64_t kind_counter = 0;
    recursionNodeParse(node, kind_counter, PROGRAM);

    for (NCASM_Instruction inst : CompiledCode) {
        std::cout << inst.OPcode;

        for (auto c : inst.Args)
        {
            std:: cout << "  " << c;
        }
        std::cout << "\n";
    }
}

void Compiler_to_NCASM::recursionNodeParse(Parser::Node *node, uint64_t &k_c,
                                           nodeType type_of_parrent, NCASM_Instruction *temp_instr)
{
    u_int64_t old_size = 0;
    nodeType new_parrent_type = type_of_parrent;
    if (node->Type == "DEFINE_VAR")
    {
        new_parrent_type = DEF_VAR;
        addInstrToCompiledCode("PUSH_TO_STACK", node->kind[0]->Info, node->kind[1]->Info);
        return;
    }
    else if (node->Type == "ASSIGN")
    {
        new_parrent_type == ASSIGN;
        addInstrToCompiledCode("LOAD", node->kind[0]->Info, "tmp_r0");

        if (node->kind[1]->Type == "VALUE") {
            addInstrToCompiledCode("SET", node->kind[1]->Info, "tmp_r0");
        }
        else if (node->kind[1]->Type == "VAR_NAME") {
            addInstrToCompiledCode("LOAD", node->kind[1]->Info, "tmp_r1");
            addInstrToCompiledCode("COPY", "tmp_r1", "tmp_r0");
        }
        else {
            recursionMathExpCompiler(node->kind[1], -1);

            addInstrToCompiledCode("COPY", "math_r0", "tmp_r0");
        }
        return;
    }
    else if (node->Type == "IF" or node->Type == "WHILE")
    {
        std::string arg_type = node->kind[0]->kind[0]->Type;
        if (node->Type == "WHILE")
            addInstrToCompiledCode("WHILE_BEGIN");
        else if (node->Type == "IF")
            addInstrToCompiledCode("IF_BEGIN");

        if (arg_type == "VALUE" or arg_type == "VAR_NAME") {
            if (arg_type == "VALUE") {
                addInstrToCompiledCode("SET", node->kind[0]->kind[0]->Info, "math_r0");
            }
            else
            {
                addInstrToCompiledCode("LOAD", node->kind[0]->kind[0]->Info, "tmp_r0");
                addInstrToCompiledCode("COPY", "tmp_r0", "math_r0");
            }

            addInstrToCompiledCode("CONVERT_TO_LOGICAL", "math_r0", "math_r0");
            if (node->Type == "IF") {
                addInstrToCompiledCode("JNEQ", "math_r0", "ELSE_BEGIN");
            }
            else if (node->Type == "WHILE") {
                addInstrToCompiledCode("JNEQ", "math_r0", "WHILE_END");
            }
        }
        else {
            recursionMathExpCompiler(node->kind[0]->kind[0], -1);

            arg_type = CompiledCode[CompiledCode.size() - 1].OPcode;
            if (arg_type == "ADD" or arg_type == "MINUS" or arg_type == "MULTIPLE" or arg_type == "DIVIDE")
            {
                addInstrToCompiledCode("CONVERT_TO_LOGICAL", "math_r0", "math_r0");
                addInstrToCompiledCode("JNEQ", "math_r0", "ELSE_BEGIN");
            }
        }

        old_size = CompiledCode.size();
    }
    else if (node->Type == "FN_CALL" or node->Info == "print")
    {
        if (node->kind[0]->Type == "VALUE") {
            addInstrToCompiledCode("SET", node->kind[0]->Info, "tmp_r0");
        }
        else if (node->kind[0]->Type == "VAR_NAME") {
            addInstrToCompiledCode("LOAD", node->kind[0]->Info, "tmp_r0");
        }

        addInstrToCompiledCode("PRINT", "tmp_r0");
    }

    ++k_c;
    for (Parser::Node *kind : node->kind)
    {
        if (kind != nullptr)
        {
            if (kind->Type == "ARGS") {
                continue;
            }
            else
                recursionNodeParse(kind, k_c, new_parrent_type, temp_instr);
        }
    }
    --k_c;
    if (node->Type == "IF") {
        auto f = std::find(node->Mother->kind.begin(), node->Mother->kind.end(), node);
        if (*f != nullptr and (*f)->Type == "ELSE")
            addInstrToCompiledCode("JMP", "ELSE_END");

        addInstrToCompiledCode("ELSE_BEGIN");
    }
    if (node->Type == "WHILE") {
        addInstrToCompiledCode("JMP", "WHILE_BEGIN");
        addInstrToCompiledCode("WHILE_END");
    }
    else if (node->Type == "ELSE") {
        addInstrToCompiledCode("ELSE_END");
    }
}

void Compiler_to_NCASM::recursionMathExpCompiler(Parser::Node *node, int64_t num, bool is_logical)
{
    std::string val1 = "";
    std::string val2 = "";
    bool pass_val1 = false;
    bool pass_val2 = false;
    int64_t arg_num = 0;
    for (Parser::Node *kind : node->kind)
    {
        if (kind != nullptr) {
            if (kind->Type == "VALUE") {
                if (arg_num == 0)
                    val1 = kind->Info;
                else if (arg_num == 1)
                    val2 = kind->Info;
            }
            else if (kind->Type == "VAR_NAME") {
                if (arg_num == 0) {
                    addInstrToCompiledCode("LOAD", kind->Info, "tmp_r0");
                    addInstrToCompiledCode("COPY", "tmp_r0", "math_r0");
                    pass_val1 = true;
                }
                else if (arg_num == 1)
                {
                    addInstrToCompiledCode("LOAD", kind->Info, "tmp_r0");
                    addInstrToCompiledCode("COPY", "tmp_r0", "math_r1");
                    pass_val2 = true;
                }
            }
            else {
                recursionMathExpCompiler(kind, arg_num);
            }
        }
        ++arg_num;
    }

    if (!val1.empty()) {
        addInstrToCompiledCode("SET", val1, "math_r0");
    }
    else if (!pass_val1) {
        addInstrToCompiledCode("MOV", "math_r2", "math_r0");
    }

    if (!val2.empty()) {
        addInstrToCompiledCode("SET", val2, "math_r1");
    }
    else if (!pass_val2 and node->Type != "LNOT") {
        addInstrToCompiledCode("MOV", "math_r3", "math_r1");
    }

    if (node->Type == "ADD" or node->Type == "MINUS" or node->Type == "MULTIPLE" or node->Type == "DIVIDE")
        addInstrToCompiledCode(node->Type, "math_r1", "math_r0");
    else if (node->Type == "LAND" or node->Type == "LOR" or node->Type == "LNOT") {
        addInstrToCompiledCode("CONVERT_TO_LOGICAL", "math_r0", "math_r0");

        if (node->Type == "LNOT")
            addInstrToCompiledCode(node->Type, "math_r0", "math_r0");
        else {
            addInstrToCompiledCode("CONVERT_TO_LOGICAL", "math1", "math1");
            addInstrToCompiledCode(node->Type, "math1", "math0");
        }
    }
    else
    {
        addInstrToCompiledCode("MINUS", "math1", "math0");
        if (node->Type == "LLESS")
            addInstrToCompiledCode("JNLESS", "math_r0", "ELSE_BEGIN");
        else if (node->Type == "LMORE")
            addInstrToCompiledCode("JNMORE", "math_r0", "ELSE_BEGIN");
        else if (node->Type == "LEQUAL")
            addInstrToCompiledCode("JNNEQ", "math_r0", "ELSE_BEGIN");
        else if (node->Type == "LNOT_EQ")
            addInstrToCompiledCode("JEQ", "math_r0", "ELSE_BEGIN");
    }

    std::string mov_to_reg = "";
    if (num == 0)
        mov_to_reg = "math_r2";
    else if (num == 1)
        mov_to_reg = "math_r3";
    else if (num == -1)
        return;

    addInstrToCompiledCode("MOV", "math_r0", mov_to_reg);
}

void Compiler_to_NCASM::addInstrToCompiledCode(std::string OPCode, std::string arg1, std::string arg2, std::string arg3)
{
    NCASM_Instruction temp_instr;
    temp_instr.OPcode = OPCode;

    if (!arg1.empty())
        temp_instr.Args.push_back(arg1);
    if (!arg2.empty())
        temp_instr.Args.push_back(arg2);
    if (!arg3.empty())
        temp_instr.Args.push_back(arg3);

    CompiledCode.push_back(temp_instr);
}

std::vector<Compiler_to_NCASM::NCASM_Instruction>* Compiler_to_NCASM::getCompiledCode() {
    return &CompiledCode;
}
