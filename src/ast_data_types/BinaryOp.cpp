#include "../../include/ast_data_types/BinaryOp.h"

// BinaryOp
string Add::to_string() const {
    return "Add";
}

string Sub::to_string() const {
    return "Sub";
}

string Mul::to_string() const {
    return "Mul";
}

string Div::to_string() const {
    return "Div";
}

string Equal::to_string() const {
    return "Equal";
}

string NotEq::to_string() const {
    return "NotEq";
}

string Lt::to_string() const {
    return "Lt";
}

string Lte::to_string() const {
    return "Lte";
}

string Gt::to_string() const {
    return "Gt";
}

string Gte::to_string() const {
    return "Gte";
}