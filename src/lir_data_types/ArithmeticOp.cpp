#include "../../include/lir_data_types/ArithmeticOp.h"

AddLIR::AddLIR() {}
string AddLIR::to_string() const {
    return "add";
}

SubLIR::SubLIR() {}
string SubLIR::to_string() const {
    return "sub";
}

MultLIR::MultLIR() {}
string MultLIR::to_string() const {
    return "mul";
}

DivLIR::DivLIR() {}
string DivLIR::to_string() const {
    return "div";
}