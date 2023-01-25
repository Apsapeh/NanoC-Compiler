#include "Compiler_to_NCASM.h"

Compiler_to_NCASM::Compiler_to_NCASM(Parser::Node* node) {

    uint64_t kind_counter = 0;
    recursionNodeParse(node, kind_counter, PROGRAM);

    std::cout << CompiledCode.size() << std::endl;
    for (NCASM_Instruction inst : CompiledCode) {
        std::cout << inst.OPcode << std::endl;
    }
}

void Compiler_to_NCASM::recursionNodeParse(Parser::Node *node, uint64_t &k_c,
                                           nodeType type_of_parrent, NCASM_Instruction *temp_instr) {
    nodeType new_parrent_type = type_of_parrent;
    if (node->Type == "DEFINE_VAR")
    {
        new_parrent_type = DEF_VAR;
        temp_instr = new NCASM_Instruction;
        temp_instr->OPcode = "PUSH_TO_STACK";
    }

    switch (type_of_parrent) {
        case DEF_VAR:
            //temp_instr->Args.push_back(node->Info);
            std::cout << k_c << " | " << node->Type << " | " << node->Info << std::endl;
            break;
    }

    ++k_c;
    for (Parser::Node *kind : node->kind)
    {
        if (kind != nullptr)
        {
            recursionNodeParse(kind, k_c, new_parrent_type, temp_instr);

        }
    }
    --k_c;
    if (temp_instr != nullptr) {
        CompiledCode.push_back(*temp_instr);
        //delete temp_instr;
    }
}