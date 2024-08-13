#ifndef UNARYOP_H
#define UNARYOP_H

#include <string>

using namespace std;

// UnaryOp
struct UnaryOp {
    virtual ~UnaryOp() {}
    virtual string to_string() const = 0;
};
struct Neg : UnaryOp {
    string to_string() const override;
};
struct Deref : UnaryOp {
    string to_string() const override;
};

#endif