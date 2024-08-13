#ifndef RHS_H
#define RHS_H

#include <string>
#include <memory>
#include "Exp.h"
#include "Type.h"

using namespace std;

// Rhs
struct Rhs {
    virtual ~Rhs() {}
    virtual string to_string() const = 0;
    virtual string get_type() const = 0;
};
struct RhsExp : Rhs {
    Exp* exp;

    RhsExp(Exp* exp);
    ~RhsExp();
    string to_string() const override;
    string get_type() const override;
};
struct New : Rhs {
    shared_ptr<Type> type;
    Exp* amount;

    New(shared_ptr<Type> type, Exp* amount);
    ~New();
    string to_string() const override;
    string get_type() const override;
};

#endif