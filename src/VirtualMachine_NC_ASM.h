#pragma once

/*
 * alc <size>
*/

class VirtualMachine_NC_ASM {
    struct ASM_Instruction {
        char Command[3];
        unsigned int reg_index_one;
        unsigned int reg_index_two;
        /*char *reg1;
        char *reg2;*/
    };
    VirtualMachine_NC_ASM();
};