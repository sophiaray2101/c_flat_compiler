#include "../../include/lir_data_types/LirInstructions.h"
#include <iostream>

Label::Label() {}
Label::Label(string label, string name) : label(label), name(name) {}
string Label::to_string() const {
    return "Label(" + this->label + ", " + this->name + ")";
}
string Label::get_type() const {
    return "LABEL";
}

Alloc::~Alloc() {
    delete this->num;
    this->num = nullptr;
}
Alloc::Alloc() {}
Alloc::Alloc(string lhs, Operand* num) : lhs(lhs), num(num) {}
string Alloc::to_string() const {
    return "Alloc(" + this->lhs + ", " + this->num->to_string() + ")";
}
string Alloc::get_type() const {
    return "ALLOC";
}

Arith::~Arith() {
    delete this->aop;
    this->aop = nullptr;
    delete this->left;
    this->left = nullptr;
    delete this->right;
    this->right = nullptr;
}
Arith::Arith() {}
Arith::Arith(string lhs, ArithmeticOp* aop, Operand* left, Operand* right) : lhs(lhs), aop(aop), left(left), right(right) {}
string Arith::to_string() const {
    return "Arith(" + this->lhs + ", " + this->aop->to_string() + ", " + this->left->to_string() + ", " + this->right->to_string() + ")";
}
string Arith::get_type() const {
    return "ARITH";
}

CallExt::~CallExt() {
    delete this->lhs;
    this->lhs = nullptr;
    for (Operand* o : this->args) {
        delete o;
        o = nullptr;
    }
}
CallExt::CallExt() {}
CallExt::CallExt(string* lhs, string callee, vector<Operand*>& args) : lhs(lhs), callee(callee), args(args) {}
string CallExt::to_string() const {
    string lhs_var = (!this->lhs) ? "_" : *this->lhs;
    string val = "CallExt(" + lhs_var + ", " + this->callee + ", [";
    bool first = true;
    for (Operand* arg : this->args) {
        if (!first) {
            val += ", ";
        }
        val += arg->to_string();
        first = false;
    }
    val += "])";
    return val;
}
string CallExt::get_type() const {
    return "CALLEXT";
}

Cmp::~Cmp() {
    delete this->aop;
    this->aop = nullptr;
    delete this->left;
    this->left = nullptr;
    delete this->right;
    this->right = nullptr;
}
Cmp::Cmp() {}
Cmp::Cmp(string lhs, ComparisonOp* aop, Operand* left, Operand* right) : lhs(lhs), aop(aop), left(left), right(right) {}
string Cmp::to_string() const {
    return "Cmp(" + this->lhs + ", " + this->aop->to_string() + ", " + this->left->to_string() + ", " + this->right->to_string() + ")";
}
string Cmp::get_type() const {
    return "CMP";
}

Copy::~Copy() {
    delete this->op;
    this->op = nullptr;
}
Copy::Copy() {}
Copy::Copy(string lhs, Operand* op) : lhs(lhs), op(op) {}
string Copy::to_string() const {
    return "Copy(" + this->lhs + ", " + this->op->to_string() + ")";
}
string Copy::get_type() const {
    return "COPY";
}

Gep::~Gep() {
    delete this->idx;
    this->idx = nullptr;
}
Gep::Gep() {}
Gep::Gep(string lhs, string src, Operand* idx) : lhs(lhs), src(src), idx(idx) {}
string Gep::to_string() const {
    return "Gep(" + this->lhs + ", " + this->src + ", " + this->idx->to_string() + ")";
}
string Gep::get_type() const {
    return "GEP";
}

Gfp::Gfp() {}
Gfp::Gfp(string lhs, string src, string field) : lhs(lhs), src(src), field(field) {}
string Gfp::to_string() const {
    return "Gfp(" + this->lhs + ", " + this->src + ", " + this->field + ")";
}
string Gfp::get_type() const {
    return "GFP";
}

Load::Load() {}
Load::Load(string lhs, string src) : lhs(lhs), src(src) {}
string Load::to_string() const {
    return "Load(" + this->lhs + ", " + this->src + ")";
}
string Load::get_type() const {
    return "LOAD";
}

Store::~Store() {
    delete this->op;
    this->op = nullptr;
}
Store::Store() {}
Store::Store(string dst, Operand* op) : dst(dst), op(op) {}
string Store::to_string() const {
    return "Store(" + this->dst + ", " + this->op->to_string() + ")";
}
string Store::get_type() const {
    return "STORE";
}


Branch::~Branch() {
    delete this->guard;
    this->guard = nullptr;
}
Branch::Branch() {}
Branch::Branch(Operand* guard, shared_ptr<Label> tt, shared_ptr<Label> ff) : guard(guard), tt(tt), ff(ff) {}
string Branch::to_string() const {
    return "Branch(" + this->guard->to_string() + ", " + this->tt->name + ", " + this->ff->name + ")";
}
string Branch::get_type() const {
    return "BRANCH";
}

CallDirect::~CallDirect() {
    delete this->lhs;
    this->lhs = nullptr;
    for (Operand* o : this->args) {
        delete o;
        o = nullptr;
    }
}
CallDirect::CallDirect() {}
CallDirect::CallDirect(string* lhs, string callee, vector<Operand*>& args, string next_bb) : lhs(lhs), callee(callee), args(args), next_bb(next_bb) {}
string CallDirect::to_string() const {
    string lhs_var = (!this->lhs) ? "_" : *this->lhs;
    string val = "CallDirect(" + lhs_var + ", " + this->callee + ", [";
    bool first = true;
    for (Operand* arg : this->args) {
        if (!first) {
            val += ", ";
        }
        val += arg->to_string();
        first = false;
    }
    val += "], " + this->next_bb + ")";
    return val;
}
string CallDirect::get_type() const {
    return "CALLDIRECT";
}

CallIndirect::~CallIndirect() {
    delete this->lhs;
    this->lhs = nullptr;
    for (Operand* o : this->args) {
        delete o;
        o = nullptr;
    }
}
CallIndirect::CallIndirect() {}
CallIndirect::CallIndirect(string* lhs, string callee, vector<Operand*>& args, string next_bb) : lhs(lhs), callee(callee), args(args), next_bb(next_bb) {}
string CallIndirect::to_string() const {
    string lhs_var = (!this->lhs) ? "_" : *this->lhs;
    string val = "CallIndirect(" + lhs_var + ", " + this->callee + ", [";
    bool first = true;
    for (Operand* arg : this->args) {
        if (!first) {
            val += ", ";
        }
        val += arg->to_string();
        first = false;
    }
    val += "], " + this->next_bb + ")";
    return val;
}
string CallIndirect::get_type() const {
    return "CALLINDIRECT";
}

Jump::Jump() {}
Jump::Jump(shared_ptr<Label> next_bb) : next_bb(next_bb) {}
string Jump::to_string() const {
    return "Jump(" + this->next_bb->name + ")";
}
string Jump::get_type() const {
    return "JUMP";
}

Ret::~Ret() {
    delete this->op;
    this->op = nullptr;
}
Ret::Ret() {}
Ret::Ret(Operand* op) : op(op) {}
string Ret::to_string() const {
    string ret_val = (!this->op) ? "_" : this->op->to_string();
    return "Ret(" + ret_val + ")";
}
string Ret::get_type() const {
    return "RET";
}