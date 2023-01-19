//#include <malloc.h>
#include <iostream>
#include <stdlib.h>
#include "VirtualMachine_NC_ASM.h"


VirtualMachine_NC_ASM::VirtualMachine_NC_ASM (VirtualMachine_NC_ASM::ASM_Instruction* instructions, unsigned long long count_of_instrs, unsigned long long stack_size)
{
    //void* stack[stack_size];
    void** dyn_stack = (void**)malloc(0);
    void** heap = (void**)malloc(0);
    u_int64_t heap_size = 0;

    ASM_Instruction *inst;
    for (unsigned long long num_of_instr = 0; num_of_instr < count_of_instrs; ++num_of_instr) {
        inst = &instructions[num_of_instr];

        if (inst->Command == 1) {
            ++heap_size;
            heap = (void **)realloc(heap, sizeof(void*) * heap_size);
            heap[*(u_int64_t *)inst->val1] = malloc(*(u_int64_t*)inst->val2);
        }
        else if (inst->Command == 2) {
            free(heap[*(u_int64_t *)inst->val1]);
            heap[*(u_int64_t *)inst->val1] = nullptr;
        }
        else if (inst->Command == 3) {

        }
        else if (inst->Command == 5) {
            *(u_int64_t*)stack[*(u_int64_t *)inst->val1] = *(u_int64_t*)inst->val2;
            //std::cout << *(u_int64_t*)inst->val2 << std::endl;

        }
        else if (inst->Command == 6) {
            std::cout << *(u_int64_t*)stack[*(u_int64_t *)inst->val1] << std::endl;
        }
    }
}