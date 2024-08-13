#ifndef BINARYOP_H
#define BINARYOP_H

#include <string>

using namespace std;

// BinaryOp
struct BinaryOp {
    virtual ~BinaryOp() {}
    virtual string to_string() const = 0;
};
struct Add : BinaryOp {
    string to_string() const override;
};
struct Sub : BinaryOp {
    string to_string() const override;
};
struct Mul : BinaryOp {
    string to_string() const override;
};
struct Div : BinaryOp {
    string to_string() const override;
};
struct Equal : BinaryOp {
    string to_string() const override;
};
struct NotEq : BinaryOp {
    string to_string() const override;
};
struct Lt : BinaryOp {
    string to_string() const override;
};
struct Lte : BinaryOp {
    string to_string() const override;
};
struct Gt : BinaryOp {
    string to_string() const override;
};
struct Gte : BinaryOp {
    string to_string() const override;
};

#endif