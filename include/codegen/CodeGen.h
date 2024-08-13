#ifndef CODEGEN_H
#define CODEGEN_H

#include <map>
#include <string>
#include "../lowering/lowering.h"
#include "DataCG.h"
#include "FunctionCG.h"

using namespace std;

// CodeGen
struct CodeGen {
    map<string, DataCG*> data;
    vector<string> error_messages;
    map<string, FunctionCG*> functions;
    vector<BlockCG*> error_blocks;

    CodeGen(ProgramLIR& programLIR);
    ~CodeGen();
    string to_string() const;
};

#endif
