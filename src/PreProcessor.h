#pragma once

#include <string>
#include "Compiler_NCASM_to_ByteCode.h"

class PreProcessor {
public:
    PreProcessor(std::string source);

private:
    struct DefineStruct {
        std::string Name;
        std::vector<std::string> Args;
        std::vector<std::string> Body;
    };

    std::vector<std::string> changeDefineToString(DefineStruct def_data, std::vector<std::string> &args_line);
    int64_t getDefineIndex(std::string str, std::vector<DefineStruct> *def_vec);

    void AddDefine(std::string &word, std::vector<DefineStruct> &defines, u_int32_t &define_index,
                            u_int32_t &local_rbrac_counter, std::vector<std::string> &tmp_vec_for_args,
                            std::vector<std::string> &vec_to_push_result);

    std::vector<std::string> req(std::vector<std::string>&, uint32_t, std::vector<DefineStruct>*);
};

// Hello, my name is Tofik