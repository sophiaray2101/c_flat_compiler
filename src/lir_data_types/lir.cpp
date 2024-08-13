#include "../../include/lir_data_types/lir.h"

#include <iostream>

BasicBlockLIR::~BasicBlockLIR() {
}
BasicBlockLIR::BasicBlockLIR() {}
string BasicBlockLIR::to_string() const {
    string val = "";
    for (auto inst : this->insts) {
        val += "\n    " + inst->to_string();
    }
    val += "\n    " + this->term->to_string();
    return val;
}

FunctionLIR::~FunctionLIR() {
    for (auto mapping : this->body) {
        delete mapping.second;
        mapping.second = nullptr;
    }
}
FunctionLIR::FunctionLIR() {}
string FunctionLIR::to_string() const {
    string rettyp_val = (!this->rettyp) ? "_" : this->rettyp->to_string();
    string val = "(";
    bool first = false;
    for (pair<string, shared_ptr<Type>> prm : this->params) {
        if (first) {
            val += ", ";
        }
        val += prm.first + ":" + prm.second->to_string();
        first = true;
    }
    val += ") -> " + rettyp_val + " {\n  Locals";

    for (auto loc : this->locals) {
        val += "\n    " + loc.first + " : " + loc.second->to_string();
    }

    for (auto block : this->body) {
        val += "\n\n  " + block.first + ":" + block.second->to_string();
    }

    val += "\n}";
    return val;
}