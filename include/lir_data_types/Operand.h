#ifndef OPERAND_H
#define OPERAND_H

#include <cstdint>
#include <string>
#include "../../include/ast_data_types/Type.h"

using namespace std;

// Operand
struct Operand {
    virtual string to_string() const = 0;
    virtual string get_type() const = 0;
    virtual ~Operand() {}
};

struct Const : Operand {
    int32_t num;

    Const();
    Const(int32_t num);
    string to_string() const override;
    string get_type() const override;
};

struct Var : Operand {
    string id;
    
    Var();
    Var(string id);
    string to_string() const override;
    string get_type() const override;
};

#endif