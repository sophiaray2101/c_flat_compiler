#include "../../include/lir_data_types/Operand.h"

Const::Const() {}
Const::Const(int32_t num) : num(num) {}
string Const::to_string() const {
    return std::to_string(this->num);
}
string Const::get_type() const{
    return "CONST";
}

Var::Var() {}
Var::Var(string id) : id(id) {}
string Var::to_string() const {
    return this->id;
}
string Var::get_type() const{
    return "VAR";
}