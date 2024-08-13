#include "../../include/ast_data_types/UnaryOp.h"

// UnaryOp
string Neg::to_string() const {
    return "Neg";
}

string Deref::to_string() const {
    return "Deref";
}