#ifndef EXP_H
#define EXP_H

#include <string>
#include <vector>
#include "UnaryOp.h"
#include "BinaryOp.h"

using namespace std;

// Exp
struct Exp {
    virtual ~Exp() {}
    virtual string to_string() const = 0;
    virtual string get_type() const = 0;
};
struct NumExp : Exp {
    int32_t n;

    NumExp(int32_t n);
    string to_string() const override;
    string get_type() const override;
};
struct IdExp : Exp {
    string name;

    IdExp(string name);
    string to_string() const override;
    string get_type() const override;
};
struct NilExp : Exp {
    string to_string() const override;
    string get_type() const override;
};
struct UnOpExp : Exp {
    UnaryOp* op;
    Exp* operand;

    UnOpExp();
    UnOpExp(UnaryOp* op, Exp* operand);
    ~UnOpExp() override;
    string to_string() const override;
    string get_type() const override;
};
struct BinOpExp : Exp {
    BinaryOp* op;
    Exp* left;
    Exp* right;

    BinOpExp(BinaryOp* op, Exp* left, Exp* right);
    ~BinOpExp() override;
    string to_string() const override;
    string get_type() const override;
};
struct ArrayAccessExp : Exp {
    Exp* ptr;
    Exp* index;

    ArrayAccessExp(Exp* ptr, Exp* index);
    ~ArrayAccessExp() override;
    string to_string() const override;
    string get_type() const override;
};
struct FieldAccessExp : Exp {
    Exp* ptr;
    string field;

    FieldAccessExp(Exp* ptr, string field);
    ~FieldAccessExp() override;
    string to_string() const override;
    string get_type() const override;
};
struct CallExp : Exp {
    Exp* callee;
    vector<Exp*> args;

    CallExp(Exp* callee, vector<Exp*>& args);
    ~CallExp() override;
    string to_string() const override;
    string get_type() const override;
};

#endif