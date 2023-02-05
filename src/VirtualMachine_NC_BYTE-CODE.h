#pragma once
#include <cstdint>

/*
 * 1 - allocate memory                      (adr) (size)
 * 2 - load from heap to r1                 (adr) (r1)
 * 3 - free memory                          (adr)
 * 4 - rebase heap
 *
 * 5 - add to dynamic stack                 (size)
 * 6 - load from dynamic stack              (adr) (r1)
 * 7 - del from dynamic stack               (size)
 *
 * 8 - copy from r1 to r2                   (r1) (r2)
 * 9 - assign by link r1 to r2              (r1) (r2)
 * 10 - set value to reg                    (value) (r1)
 *
 * 11 - add r2 to r1                        (r2) (r1)
 * 12 - subdivide r2 from r1                (r2) (r1)
 * 13 - multiple r1 by r2                   (r2) (r1)
 * 14 - divide r1 by r2                     (r2) (r1)
 *
 * 15 - jump on val bytes if r1 = 1         (val) (r1)
 * 16 - jump on val bytes if r1 = 0         (vol) (r1)
 * 17 - jump on val bytes if r1 < 0         (val) (r1)
 * 18 - jump on val bytes if r1 > 0         (val) (r1)
 * 19 - jump on val bytes if not (r1 = 1)   (val) (r1)
 * 20 - jump on val bytes if not (r1 = 0)   (val) (r1)
 * 21 - jump on val bytes if not (r1 < 0)   (val) (r1)
 * 22 - jump on val bytes if not (r1 > 0)   (val) (r1)
 * 23 - jump on val bytes                   (val)
 *
 * 24 - convert r1 to logical 1 or 0        (r1) (r2)
 * 25 - (AND) if r1 + r2 == 2 write 1 to r2 (r1) (r2)
 * 26 - (OR)  if r2 + r1 != 0 write 1 to r2 (r1) (r2)
 * 27 - (NOT) !r1 write to r2               (r1) (r2)
 *
 * 50 - print r1
 * 51 - set val as new
*/

typedef uint64_t u_int64_t;
typedef uint16_t u_int16_t;

class VirtualMachine_NC_BYTE {
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
    VirtualMachine_NC_BYTE(ASM_Instruction* instructions, unsigned long long count_of_instrs,
                           unsigned long long stack_size, unsigned short heap_chunk);
private:
    struct heapNullChunk {
        u_int16_t count = 0;
        u_int64_t pos:48;
    };

    struct varsBind {
        u_int64_t name;
        u_int64_t index;
    };

    inline u_int64_t binarySearchinHeapNull(heapNullChunk *arr, u_int64_t arr_size, u_int64_t chunk_pos) ;
};