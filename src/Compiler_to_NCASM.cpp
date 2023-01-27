#include "Compiler_to_NCASM.h"

Compiler_to_NCASM::Compiler_to_NCASM(Parser::Node* node)
{

    uint64_t kind_counter = 0;
    recursionNodeParse(node, kind_counter, PROGRAM);

    for (NCASM_Instruction inst : CompiledCode) {
        std::cout << inst.OPcode;

        for (auto c : inst.Args)
        {
            std:: cout << " | " << c;
        }
        std::cout << "\n";
    }
}

void Compiler_to_NCASM::recursionNodeParse(Parser::Node *node, uint64_t &k_c,
                                           nodeType type_of_parrent, NCASM_Instruction *temp_instr)
{
    nodeType new_parrent_type = type_of_parrent;
    if (node->Type == "DEFINE_VAR")
    {
        new_parrent_type = DEF_VAR;
        temp_instr = new NCASM_Instruction;
        temp_instr->OPcode = "PUSH_TO_STACK";
        temp_instr->Args.push_back(node->kind[0]->Info);
        temp_instr->Args.push_back(node->kind[1]->Info);
        CompiledCode.push_back(*temp_instr);
        delete temp_instr;
        return;
    }
    else if (node->Type == "ASSIGN")
    {
        new_parrent_type == ASSIGN;
        temp_instr = new NCASM_Instruction;
        temp_instr->OPcode = "LOAD";
        temp_instr->Args.push_back(node->kind[0]->Info);
        temp_instr->Args.push_back("tmp_r0");
        CompiledCode.push_back(*temp_instr);
        delete temp_instr;

        if (node->kind[1]->Type == "VALUE") {
            temp_instr = new NCASM_Instruction;
            temp_instr->OPcode = "SET";
            temp_instr->Args.push_back(node->kind[1]->Info);
            temp_instr->Args.push_back("tmp_r0");
            CompiledCode.push_back(*temp_instr);
            delete temp_instr;
        }
        else if (node->kind[1]->Type == "VAR_NAME") {
            temp_instr = new NCASM_Instruction;
            temp_instr->OPcode = "LOAD";
            temp_instr->Args.push_back(node->kind[1]->Info);
            temp_instr->Args.push_back("tmp_r1");
            CompiledCode.push_back(*temp_instr);
            delete temp_instr;

            temp_instr = new NCASM_Instruction;
            temp_instr->OPcode = "COPY";
            temp_instr->Args.push_back("tmp_r1");
            temp_instr->Args.push_back("tmp_r0");
            CompiledCode.push_back(*temp_instr);
            delete temp_instr;
        }
        else {
            recursionMathExpCompiler(node->kind[1], -1);

            temp_instr = new NCASM_Instruction;
            temp_instr->OPcode = "COPY";
            temp_instr->Args.push_back("math_r0");
            temp_instr->Args.push_back("tmp_r0");
            CompiledCode.push_back(*temp_instr);
            delete temp_instr;
        }
            return;
    }
    else if (node->Type == "IF")
    {
        std::cout << "GAY" << std::endl;
    }

    /*switch (type_of_parrent) {
        case DEF_VAR:
            temp_instr->Args.push_back(node->Info);
            break;
        case ASSIGN:

            break;
    }*/

    ++k_c;
    for (Parser::Node *kind : node->kind)
    {
        if (kind != nullptr)
        {
            recursionNodeParse(kind, k_c, new_parrent_type, temp_instr);
        }
    }
    --k_c;
    if (temp_instr != nullptr and node->kind.size() != 0) {
        CompiledCode.push_back(*temp_instr);
        delete temp_instr;
    }
}

/*
ADD ()
	VALUE (5)
	ADD ()
		VALUE (3)
		MINUS ()
			VALUE (2)
			VALUE (1)

1 - MINUS 2 1
2 - ADD 3 1.
3 - ADD 5 2.

set 2 r0
set 1 r1
minus r1 r0
mov r0 r3

set 3 r0
mov r3 r1
add r1 r0
mov r0 r3

set 5 r0
mov r3 r1
add r1 r0
mov r0 r3
*/

void Compiler_to_NCASM::recursionMathExpCompiler(Parser::Node *node, int64_t num)
{
    std::cout << node->Type << std::endl;
    std::string val1 = "";
    std::string val2 = "";
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
            else
                recursionMathExpCompiler(kind, arg_num);
        }

        ++arg_num;
    }

    std::cout << val1 << " | " << val2 << std::endl;
    if (!val1.empty()) {
        NCASM_Instruction* temp_instr = new NCASM_Instruction;
        temp_instr->OPcode = "SET";
        temp_instr->Args.push_back(val1);
        temp_instr->Args.push_back("math_r0");
        CompiledCode.push_back(*temp_instr);
        delete temp_instr;
    }
    else {
        NCASM_Instruction* temp_instr = new NCASM_Instruction;
        temp_instr->OPcode = "MOV";
        temp_instr->Args.push_back("math_r2");
        temp_instr->Args.push_back("math_r0");
        CompiledCode.push_back(*temp_instr);
        delete temp_instr;
    }


    if (!val2.empty()) {
        NCASM_Instruction* temp_instr = new NCASM_Instruction;
        temp_instr->OPcode = "SET";
        temp_instr->Args.push_back(val2);
        temp_instr->Args.push_back("math_r1");
        CompiledCode.push_back(*temp_instr);
        delete temp_instr;
    }
    else {
        NCASM_Instruction* temp_instr = new NCASM_Instruction;
        temp_instr->OPcode = "MOV";
        temp_instr->Args.push_back("math_r3");
        temp_instr->Args.push_back("math_r1");
        CompiledCode.push_back(*temp_instr);
        delete temp_instr;
    }

    NCASM_Instruction* temp_instr = new NCASM_Instruction;
    temp_instr->OPcode = node->Type;
    temp_instr->Args.push_back("math_r1");
    temp_instr->Args.push_back("math_r0");
    CompiledCode.push_back(*temp_instr);
    delete temp_instr;

    std::string mov_to_reg = "";
    if (num == 0)
        mov_to_reg = "math_r2";
    else if (num == 1)
        mov_to_reg = "math_r3";
    else if (num == -1)
        return;


    temp_instr = new NCASM_Instruction;
    temp_instr->OPcode = "MOV";
    temp_instr->Args.push_back("math_r0");
    temp_instr->Args.push_back(mov_to_reg);
    CompiledCode.push_back(*temp_instr);
    delete temp_instr;
}