#include "../../include/codegen/XOps.h"

DollSign::DollSign(int val) : val(val) {}
string DollSign::get_type() const {
    return "DollSign";
}
string DollSign::to_string() const {
    return "$" + std::to_string(this->val);
}

Reg::Reg(string val) : val(val) {}
string Reg::get_type() const {
    return "Reg";
}
string Reg::to_string() const {
    return "%" + this->val;
}

OffReg::OffReg(string offset, shared_ptr<Reg> reg) : offset(offset), reg(reg) {}
string OffReg::get_type() const {
    return "OffReg";
}
string OffReg::to_string() const {
    return offset + "(" + this->reg->to_string() + ")";
}