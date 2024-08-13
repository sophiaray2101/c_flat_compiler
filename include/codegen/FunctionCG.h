#ifndef FUNCTIONCG_H
#define FUNCTIONCG_H

#include <string>
#include <unordered_map>
#include <map>
#include "XInsts.h"
#include "../lir_data_types/lir.h"
#include "../lowering/lowering.h"

using namespace std;

// BlockCG
struct BlockCG {
    string name;
    vector<XInsts*> insts;

    BlockCG();
    ~BlockCG();
    string to_string() const;
};

// FunctionCG
struct FunctionCG {
    string name;
    unordered_map<string, shared_ptr<XOps>> locals;
    BlockCG* prologue;
    map<string, BlockCG*> blocks;
    BlockCG* epilogue;

    void convertRetX(vector<XInsts*>& insts, Ret* ret);
    void convertBranchX(vector<XInsts*>& insts, Branch* branch);
    void convertJumpX(vector<XInsts*>& insts, Jump* jump);
    void convertCallExtX(vector<XInsts*>& insts, CallExt* callext);
    void convertArithX(vector<XInsts*>& insts, Arith* arith);
    void convertCallDirX(vector<XInsts*>& insts, CallDirect* calldir);
    void convertAllocX(vector<XInsts*>& insts, Alloc* alloc, FunctionLIR* flir, ProgramLIR& programLIR);
    void convertGepX(vector<XInsts*>& insts, Gep* gep, FunctionLIR* flir, ProgramLIR& programLIR);
    void convertLoadX(vector<XInsts*>& insts, Load* load);
    void convertStoreX(vector<XInsts*>& insts, Store* store);
    void convertGfpX(vector<XInsts*>& insts, Gfp* gfp, FunctionLIR* flir, ProgramLIR& programLIR);
    void convertCallIndirX(vector<XInsts*>& insts, CallIndirect* callindir);
    void convertX(vector<XInsts*>& insts, shared_ptr<LirInst> instLIR, FunctionLIR* flir, ProgramLIR& programLIR);
    void convertCmpX(vector<XInsts*>& insts, Cmp* cmpLIR);
    void convertCopyX(vector<XInsts*>& insts, Copy* copyLIR);
    void convertTermX(vector<XInsts*>& insts, shared_ptr<Terminal> termLIR);

    FunctionCG(FunctionLIR* flir, ProgramLIR& programLIR);
    ~FunctionCG();
    string to_string() const;
};

#endif