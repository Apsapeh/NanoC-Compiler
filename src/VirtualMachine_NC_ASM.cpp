//#include <malloc.h>
#include <iostream>
#include <map>
#include <stdlib.h>
#include "VirtualMachine_NC_ASM.h"


VirtualMachine_NC_ASM::VirtualMachine_NC_ASM (VirtualMachine_NC_ASM::ASM_Instruction* instructions, unsigned long long count_of_instrs, unsigned long long stack_size, unsigned short heap_null_chunk_size)
{
    void** dyn_stack = (void**)malloc(0);
    void** heap = (void**)malloc(0);
    heapNullChunk* heap_null_chunks_arr =  (heapNullChunk*) malloc(0);
    std::map <u_int64_t, u_int64_t> heap_vars_bind;


    void* registers[4] {nullptr, nullptr, nullptr, nullptr};

    u_int64_t dyn_stack_size = 0;
    u_int64_t dyn_stack_relative_start = 0;
    u_int64_t heap_size = 0;
    u_int64_t heap_null_arr_size = 0;

    ASM_Instruction *inst;
    for (unsigned long long num_of_instr = 0; num_of_instr < count_of_instrs; ++num_of_instr) {
        inst = &instructions[num_of_instr];
        //std::cout << (int)inst->Command << std::endl;

        // TODO: ДОДЕЛАТЬ КУЧУ
        // Heap
        if (inst->Command == 1) {
            ++heap_size;
            heap = (void **)realloc(heap, sizeof(void*) * heap_size);
            heap[*(u_int64_t *)inst->val1] = malloc(*(u_int64_t*)inst->val2);
        }
        else if (inst->Command == 3) {
            free(heap[*(u_int64_t *)inst->val1]);
            heap[*(u_int64_t *)inst->val1] = nullptr;
            u_int64_t index = binarySearchinHeapNull(heap_null_chunks_arr, heap_null_arr_size, *(u_int64_t *)inst->val1 / heap_null_chunk_size);
            ++heap_null_arr_size;
            if (!index) {
                heap_null_chunks_arr =  (heapNullChunk*) realloc(heap_null_chunks_arr, sizeof(heapNullChunk)*heap_null_arr_size);
                index = heap_null_arr_size - 1;
                heap_null_chunks_arr[index].pos = *(u_int64_t *)inst->val1;
            }
            ++heap_null_chunks_arr[index].count;
            //++heap_null_chunks_arr->count;
        }
        else if (inst->Command == 4) {
            //TODO: ADD
        }

            // Stack
        else if (inst->Command == 5) {
            ++dyn_stack_size;
            dyn_stack = (void**)realloc(dyn_stack, sizeof(void*) * dyn_stack_size);
            dyn_stack[dyn_stack_size-1] = malloc(*(u_int64_t*)inst->val1);
        }
        else if (inst->Command == 6) {
            registers[*(u_int64_t*)inst->val2] = dyn_stack[dyn_stack_relative_start + *(u_int64_t*)inst->val1];
        }
        else if (inst->Command == 7) {
            int old_size = dyn_stack_size;
            dyn_stack_size -= *(u_int64_t *)inst->val1 * sizeof(void*);
            for (u_int64_t index=dyn_stack_size; index < old_size; ++index)
                free(dyn_stack[index]);
            dyn_stack = (void**)realloc(dyn_stack, sizeof(void**) * dyn_stack_size);
        }
        else if (inst->Command == 8) {
            // TODO: ADD
        }
        else if (inst->Command == 9) {
            // TODO: ADD
            registers[*(u_int64_t*)inst->val2] = registers[*(u_int64_t*)inst->val1];
        }
        else if (inst->Command == 10) {
            registers[*(u_int64_t*)inst->val2] = inst->val1;
        }
        else if (inst->Command == 11) {
            *(u_int64_t*)registers[*(u_int64_t*)inst->val2] += *(u_int64_t*)registers[*(u_int64_t*)inst->val1];
        }
        else if (inst->Command == 12) {
            *(u_int64_t*)registers[*(u_int64_t*)inst->val2] -= *(u_int64_t*)registers[*(u_int64_t*)inst->val1];
        }
        else if (inst->Command == 13) {
            *(u_int64_t*)registers[*(u_int64_t*)inst->val2] *= *(u_int64_t*)registers[*(u_int64_t*)inst->val1];
        }
        else if (inst->Command == 14) {
            *(u_int64_t*)registers[*(u_int64_t*)inst->val2] /= *(u_int64_t*)registers[*(u_int64_t*)inst->val1];
        }
        else if (inst->Command == 15) {

        }
        else if (inst->Command == 16) {

        }
        else if (inst->Command == 17) {

        }


        else if (inst->Command == 50) {
            std::cout << *(u_int64_t*)registers[*(u_int64_t*)inst->val1] << std::endl;
        }
    }
}

inline u_int64_t VirtualMachine_NC_ASM::binarySearchinHeapNull(heapNullChunk *arr, u_int64_t arr_size, u_int64_t chunk_pos) {
    /*u_int64_t left = 0;
    u_int64_t right = 5;
    u_int64_t middle = left + (right - left) / 2;

    while (true) {
        if (arr[middle].pos < chunk_pos)
            right = middle;
        else if (arr[middle].pos > chunk_pos)
            left = middle;
        else
            return middle;
    }*/

    u_int64_t l = 0;
    u_int64_t r = 1; // в этом случае присваивается именно n
    u_int64_t mid;

    while (l < r) {
        mid = (l + r) / 2; // считываем срединный индекс отрезка [l,r]
        if (arr[mid].pos > chunk_pos) r = mid; // проверяем, какую часть нужно отбросить с поиска
        else l = mid + 1;
    }

    if (arr[r-1].pos == chunk_pos)
        return r;
    return 0;
}