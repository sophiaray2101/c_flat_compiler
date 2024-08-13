#ifndef ARITHMETICOP_H
#define ARITHMETICOP_H

#include <string>

using namespace std;

// ArithmeticOp
struct ArithmeticOp {
    virtual ~ArithmeticOp() {}
    virtual string to_string() const = 0;
};

struct AddLIR : ArithmeticOp {
    AddLIR();
    string to_string() const override;
};

struct SubLIR : ArithmeticOp {
    SubLIR();
    string to_string() const override;
};

struct MultLIR : ArithmeticOp {
    MultLIR();
    string to_string() const override;
};

struct DivLIR : ArithmeticOp {
    DivLIR();
    string to_string() const override;
};

#endif