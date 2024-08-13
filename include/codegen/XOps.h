#ifndef XOPS_H
#define XOPS_H

#include <string>
#include <memory>

using namespace std;

// XOps
struct XOps {
    virtual ~XOps() {}
    virtual string get_type() const = 0;
    virtual string to_string() const = 0;
};

struct DollSign : XOps {
    int val;

    DollSign(int val);
    string get_type() const;
    string to_string() const;
};

struct Reg : XOps {
    string val;

    Reg(string val);
    string get_type() const;
    string to_string() const;
};

struct OffReg : XOps {
    string offset;
    shared_ptr<Reg> reg;

    OffReg(string offset, shared_ptr<Reg> reg);
    string get_type() const;
    string to_string() const;
};

#endif