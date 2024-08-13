#include "../../include/ast_data_types/Rhs.h"

// Rhs
RhsExp::RhsExp(Exp* exp) : exp(exp) {}
RhsExp::~RhsExp() {
    delete this->exp;
}
string RhsExp::to_string() const {
    return this->exp->to_string();
}
string RhsExp::get_type() const {
    return "EXP";
}

New::New(shared_ptr<Type> type, Exp* amount) : type(type), amount((!amount) ? new NumExp(1) : amount) {}
New::~New() {
    delete this->amount;
}
string New::to_string() const {
    string num = (!this->amount) ? "Num(1)" : this->amount->to_string();
    return "New(" + this->type->to_string() + ", " + num + ")";
}
string New::get_type() const {
    return "NEW";
}