#include "../../include/ast_data_types/Lval.h"

// Lval
IdLval::IdLval() {}
IdLval::IdLval(string name) : name(name) {}
string IdLval::to_string() const {
    return "Id(" + this->name + ")"; 
}
string IdLval::get_type() const {
    return "ID";
}

DerefLval::DerefLval(Lval* lval) : lval(lval) {}
DerefLval::~DerefLval() {
    delete this->lval;
}
string DerefLval::to_string() const {
    return "Deref(" + this->lval->to_string() + ")";
}
string DerefLval::get_type() const {
    return "DEREF";
}

ArrayAccessLval::ArrayAccessLval(Lval* ptr, Exp* index) : ptr(ptr), index(index) {}
ArrayAccessLval::~ArrayAccessLval() {
    delete this->ptr;
    delete this->index;
}
string ArrayAccessLval::to_string() const {
    return "ArrayAccess(\n  ptr = " + this->ptr->to_string() + ",\n  index = " + this->index->to_string() + "\n)";
}
string ArrayAccessLval::get_type() const {
    return "ARRAY";
}

FieldAccessLval::FieldAccessLval(Lval* ptr, string field) : ptr(ptr), field(field) {}
FieldAccessLval::~FieldAccessLval() {
    delete this->ptr;
}
string FieldAccessLval::to_string() const {
    return "FieldAccess(\n  ptr = " + this->ptr->to_string() + ",\n  field = " + this->field + "\n)";
}
string FieldAccessLval::get_type() const {
    return "FIELD";
}
