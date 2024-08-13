#include "../../include/ast_data_types/Type.h"

// Type
string IntType::to_string() const {
    return "Int";
}
string IntType::get_type() const {
    return "int";
}
string IntType::print_type() const {
    return "int";
}

StructType::StructType() {}
StructType::StructType(const string name) : name(name) {}
string StructType::to_string() const {
    return "Struct(" + this->name + ")";
}
string StructType::get_type() const {
    return "struct_" + this->name;
}
string StructType::print_type() const {
    return this->name;
}

FnType::FnType() {}
FnType::FnType(const vector<shared_ptr<Type>>& prms, shared_ptr<Type> ret) : prms(prms), ret(ret) {}
FnType::~FnType() {
}
string FnType::to_string() const  {
    string val = "Fn([";
    string ret_type;
    if (!this->ret) {
        ret_type = "_";
    }
    else {
        ret_type = this->ret->to_string();
    }

    bool first = true;
    for (const shared_ptr<Type> prm : this->prms) {
        if (!first) {
            val += ", ";
        }
        val += prm->to_string();
        first = false;
    }
    return val + "], " + ret_type + ")";
}
string FnType::get_type() const {
    string val = "(";
    string ret_type;
    if (!this->ret) {
        ret_type = "_";
    }
    else {
        ret_type = this->ret->get_type();
    }

    bool first = true;
    for (const shared_ptr<Type> prm : this->prms) {
        if (!first) {
            val += ", ";
        }
        val += prm->get_type();
        first = false;
    }
    return val + ") -> " + ret_type;
}
string FnType::print_type() const {
    string val = "Fn([";
    string ret_type;
    if (!this->ret) {
        ret_type = "_";
    }
    else {
        ret_type = this->ret->to_string();
    }

    bool first = true;
    for (const shared_ptr<Type> prm : this->prms) {
        if (!first) {
            val += ", ";
        }
        val += prm->to_string();
        first = false;
    }
    return val + "], " + ret_type + ")";
}

PtrType::PtrType(shared_ptr<Type> ref) : ref(ref) {}
PtrType::~PtrType() {
}
string PtrType::to_string() const  {
    return "Ptr(" + this->ref->to_string() + ")";
}
string PtrType::get_type() const {
    return "&" + this->ref->get_type();
}
string PtrType::print_type() const {
    return "&" + this->ref->print_type();
}

AnyType::AnyType() {}
string AnyType::to_string() const {
    return "any";
}
string AnyType::get_type() const {
    return "_";
}
string AnyType::print_type() const {
    return "_";
}