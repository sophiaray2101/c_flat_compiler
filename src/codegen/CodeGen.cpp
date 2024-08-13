#include "../../include/codegen/CodeGen.h"
#include <memory>

CodeGen::CodeGen(ProgramLIR& programLIR) {
    for (auto global : programLIR.GlobalsLIR) {
        string name = global.first;
        shared_ptr<Type> typ = global.second;
        
        DataCG* glob = new DataCG();
        glob->name = name;
        if (typ->get_type().substr(0, 2) == "&(") {
            glob->isFunc = true;
        }
        else {
            glob->isFunc = false;
        }
        this->data[name] = glob;
    }
    
    this->error_messages = {"out_of_bounds_msg: .string \"out-of-bounds array access\"", "invalid_alloc_msg: .string \"invalid allocation amount\""};

    for (auto pair : programLIR.functionsLIR) {
        FunctionLIR* func = pair.second;
        FunctionCG* fcg = new FunctionCG(func, programLIR);

        this->functions[func->name] = fcg;
    }
    
    BlockCG* err_block_oob = new BlockCG();
    err_block_oob->name = ".out_of_bounds";
    err_block_oob->insts = {new lea(make_shared<OffReg>("out_of_bounds_msg", make_shared<Reg>("rip")), make_shared<Reg>("rdi")), new call("_cflat_panic")};

    BlockCG* err_block_ial = new BlockCG();
    err_block_ial->name = ".invalid_alloc_length";
    err_block_ial->insts = {new lea(make_shared<OffReg>("invalid_alloc_msg", make_shared<Reg>("rip")), make_shared<Reg>("rdi")), new call("_cflat_panic")};

    this->error_blocks = {err_block_oob, err_block_ial};
}

CodeGen::~CodeGen() {
    for (auto pair : this->data) {
        delete pair.second;
    }
    for (auto pair : this->functions) {
        delete pair.second;
    }
    for (auto block : this->error_blocks) {
        delete block;
    }
}

string CodeGen::to_string() const {
    string ret = ".data\n\n";
    for (auto pair : this->data) {
        ret += pair.second->to_string();
    }
    for (auto msg : this->error_messages) {
        ret += msg + "\n"; 
    }
    ret += "\n.text\n\n";
    for (auto pair : this->functions) {
        ret += pair.second->to_string();
    }
    for (auto block : this->error_blocks) {
        ret += block->to_string();
    }
    return ret;
}
