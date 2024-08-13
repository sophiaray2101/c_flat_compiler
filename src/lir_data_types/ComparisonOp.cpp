#include "../../include/lir_data_types/ComparisonOp.h"

EqualLIR::EqualLIR() {}
string EqualLIR::to_string() const {
    return "eq";
}

NotEqLIR::NotEqLIR() {}
string NotEqLIR::to_string() const {
    return "neq";
}

LtLIR::LtLIR() {}
string LtLIR::to_string() const {
    return "lt";
}

LteLIR::LteLIR() {}
string LteLIR::to_string() const {
    return "lte";
}

GtLIR::GtLIR() {}
string GtLIR::to_string() const {
    return "gt";
}

GteLIR::GteLIR() {}
string GteLIR::to_string() const {
    return "gte";
}