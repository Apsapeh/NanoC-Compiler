#include <malloc.h>
#include <iostream>
#include "VirtualMachine_NC_ASM.h"


VirtualMachine_NC_ASM::VirtualMachine_NC_ASM (VirtualMachine_NC_ASM::ASM_Instruction* instructions, unsigned long long count_of_instrs, unsigned long long stack_size)
{
    void* stack[stack_size];
    void* heap = malloc(0);

    ASM_Instruction *inst;
    for (unsigned long long num_of_instr = 0; num_of_instr < count_of_instrs; ++num_of_instr) {
        inst = &instructions[num_of_instr];

        if (inst->Command == 1) {
            stack[*(u_int64_t *)inst->val1] = malloc(*(u_int64_t*)inst->val2);
            //std::cout << *(u_int64_t*)inst->val2 << std::endl;
        }
        else if (inst->Command == 2) {
            free(stack[*(u_int64_t *)inst->val1]);
            //std::cout << " << " << *(u_int64_t *)inst->val1 << std::endl;
        }
        else if (inst->Command == 3) {
            *(u_int64_t*)stack[*(u_int64_t *)inst->val1] = *(u_int64_t*)inst->val2;
            //std::cout << *(u_int64_t*)inst->val2 << std::endl;

        }
        else if (inst->Command == 4) {
            std::cout << *(u_int64_t*)stack[*(u_int64_t *)inst->val1] << std::endl;
        }
    }
}