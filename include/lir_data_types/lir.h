#ifndef LIR_H
#define LIR_H

#include <string>
#include <map>
#include <memory>
#include "../ast_data_types/Type.h"
#include "../ast_data_types/ast.h"
#include "LirInstructions.h"

using namespace std;

// BasicBlockLIR
struct BasicBlockLIR {
    string label;
    vector<shared_ptr<LirInst>> insts;
    shared_ptr<Terminal> term;

    ~BasicBlockLIR();
    BasicBlockLIR();
    string to_string() const;
};

// FunctionLIR
struct FunctionLIR {
    string name;
    vector<pair<string, shared_ptr<Type>>> params;
    shared_ptr<Type> rettyp;
    map<string, shared_ptr<Type>> locals;
    map<string, BasicBlockLIR*> body;

    ~FunctionLIR();
    FunctionLIR();
    string to_string() const;
};

#endif