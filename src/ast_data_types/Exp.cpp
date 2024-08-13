#include "../../include/ast_data_types/Exp.h"

// Exp
NumExp::NumExp(int32_t n) : n(n) {}
string NumExp::to_string() const {
    return "Num(" + std::to_string(this->n) + ")";
}
string NumExp::get_type() const {
    return "NUM";
}

IdExp::IdExp(string name) : name(name) {}
string IdExp::to_string() const {
    return "Id(" + this->name + ")";
}
string IdExp::get_type() const {
    return "ID";
}

string NilExp::to_string() const {
    return "Nil";
}
string NilExp::get_type() const {
    return "NIL";
}

UnOpExp::UnOpExp() {}
UnOpExp::UnOpExp(UnaryOp* op, Exp* operand) : op(op), operand(operand) {}
UnOpExp::~UnOpExp() {
    delete this->op;
    delete this->operand;
}
string UnOpExp::to_string() const {
    return this->op->to_string() + "(" + this->operand->to_string() + ")";
}
string UnOpExp::get_type() const {
    if (this->op->to_string() == "Neg") {
        return "NEG";
    }
    if (this->op->to_string() == "Deref") {
        return "DEREF";
    }
    return "";
}

BinOpExp::BinOpExp(BinaryOp* op, Exp* left, Exp* right) : op(op), left(left), right(right) {}
BinOpExp::~BinOpExp() {
    delete this->op;
    delete this->left;
    delete this->right;
}
string BinOpExp::to_string() const {
    return "BinOp(\n  op = " + this->op->to_string() + ",\n  left = " + this->left->to_string() + ",\n  right = " + this->right->to_string() + "\n)"; 
}
string BinOpExp::get_type() const {
    string op_type = this->op->to_string();
    if (op_type == "Equal" || op_type == "NotEq" || op_type == "Lt" || op_type == "Lte" || op_type == "Gt" || op_type == "Gte") {
        return "BINOP-EQ";
    }
    else {
        return "BINOP-REST";
    }
}

ArrayAccessExp::ArrayAccessExp(Exp* ptr, Exp* index) : ptr(ptr), index(index) {}
ArrayAccessExp::~ArrayAccessExp() {
    delete this->ptr;
    delete this->index;
}
string ArrayAccessExp::to_string() const {
    return "ArrayAccess(\n  ptr = " + this->ptr->to_string() + ",\n  index = " + this->index->to_string() + "\n)";
}
string ArrayAccessExp::get_type() const {
    return "ARRAY";
}

FieldAccessExp::FieldAccessExp(Exp* ptr, string field) : ptr(ptr), field(field) {}
FieldAccessExp::~FieldAccessExp() {
    delete this->ptr;
}
string FieldAccessExp::to_string() const {
    return "FieldAccess(\n  ptr = " + this->ptr->to_string() + ",\n  field = " + this->field + "\n)";
}
string FieldAccessExp::get_type() const {
    return "FIELD";
}

CallExp::CallExp(Exp* callee, vector<Exp*>& args) : callee(callee), args(args) {}
CallExp::~CallExp() {
    delete this->callee;
    for (auto exp : this->args) {
        delete exp;
    }
}
string CallExp::to_string() const {
    string val = "Call(\n  callee = " + this->callee->to_string() + ",\n  args = [\n    ";
    bool first = true;
    for (const auto& exp : this->args) {
        if (!first) {
            val += ",\n    ";
        }
        val += exp->to_string();
        first = false;
    }
    val += "\n  ]\n)";
    return val;
}
string CallExp::get_type() const {
    return "ECALL";
}