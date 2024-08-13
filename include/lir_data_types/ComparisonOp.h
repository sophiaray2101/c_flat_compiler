#ifndef COMPARISONOP_H
#define COMPARISONOP_H

#include <string>

using namespace std;

// ComparisonOp
struct ComparisonOp {
    virtual ~ComparisonOp() {}
    virtual string to_string() const = 0;
};

struct EqualLIR : ComparisonOp {
    EqualLIR();
    string to_string() const override;
};

struct NotEqLIR : ComparisonOp {
    NotEqLIR();
    string to_string() const override;
};

struct LtLIR : ComparisonOp {
    LtLIR();
    string to_string() const override;
};

struct LteLIR : ComparisonOp {
    LteLIR();
    string to_string() const override;
};

struct GtLIR : ComparisonOp {
    GtLIR();
    string to_string() const override;
};

struct GteLIR : ComparisonOp {
    GteLIR();
    string to_string() const override;
};

#endif