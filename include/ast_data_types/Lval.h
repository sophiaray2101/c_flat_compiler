#ifndef LVAL_H
#define LVAL_H

#include <string>
#include "Exp.h"

using namespace std;

// Lval
struct Lval {
    virtual ~Lval() {}
    virtual string to_string() const = 0;
    virtual string get_type() const = 0;
};
struct IdLval : Lval {
    string name;
    
    IdLval();
    IdLval(string name);
    string to_string() const override;
    string get_type() const override;
};
struct DerefLval : Lval {
    Lval* lval;

    DerefLval(Lval* lval);
    ~DerefLval();
    string to_string() const override;
    string get_type() const override;
};
struct ArrayAccessLval : Lval {
    Lval* ptr;
    Exp* index;

    ArrayAccessLval(Lval* ptr, Exp* index);
    ~ArrayAccessLval();
    string to_string() const override;
    string get_type() const override;
};
struct FieldAccessLval : Lval {
    Lval* ptr;
    string field;

    FieldAccessLval(Lval* ptr, string field);
    ~FieldAccessLval();
    string to_string() const override;
    string get_type() const override;
};

#endif