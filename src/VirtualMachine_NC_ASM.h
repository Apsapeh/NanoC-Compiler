#pragma once

/*
 * 1 - allocate memory (adr) (size)
 * 2 - free memory (adr)
 * 3 - rebase heap
 * 4 - add to dynamic stack (adr) (size)
 * 5 - get from dynamic stack (adr)
 * 6 - del from dynamic stack
*/

class VirtualMachine_NC_ASM {
public:
    struct ASM_Instruction {
        unsigned char Command;
//        unsigned int index_one;
//        unsigned int index_two;
        void *val1 = nullptr;
        void *val2 = nullptr;
        /*char *reg1;
        char *reg2;*/
    };
    VirtualMachine_NC_ASM(ASM_Instruction* instructions, unsigned long long count_of_instrs,
                          unsigned long long stack_size);
};