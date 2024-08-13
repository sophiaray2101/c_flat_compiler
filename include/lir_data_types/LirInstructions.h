#ifndef LIRINSTRUCTIONS_H
#define LIRINSTRUCTIONS_H

#include <string>
#include <memory>
#include "Operand.h"
#include "ArithmeticOp.h"
#include "ComparisonOp.h"

using namespace std;

// LirInstructions
struct LirInstructions {
    virtual ~LirInstructions() {}
    virtual string to_string() const {
        return "";
    };
    virtual string get_type() const {
        return "";
    }
};

struct LirInst : LirInstructions {
    virtual ~LirInst() {}
};

struct Terminal : LirInstructions {
    virtual ~Terminal() {}
};

struct Label : LirInstructions {
    string label;
    string name;

    Label();
    Label(string label, string name);
    string to_string() const;
    string get_type() const;
};

struct Alloc : LirInst {
    string lhs;
    Operand* num;

    ~Alloc();
    Alloc();
    Alloc(string lhs, Operand* num);
    string to_string() const;
    string get_type() const;
};

struct Arith : LirInst {
    string lhs;
    ArithmeticOp* aop;
    Operand* left;
    Operand* right;

    ~Arith();
    Arith();
    Arith(string lhs, ArithmeticOp* aop, Operand* left, Operand* right);
    string to_string() const;
    string get_type() const;
};

struct CallExt : LirInst {
    string* lhs;
    string callee;
    vector<Operand*> args;

    ~CallExt();
    CallExt();
    CallExt(string* lhs, string callee, vector<Operand*>& args);
    string to_string() const;
    string get_type() const;
};

struct Cmp : LirInst {
    string lhs;
    ComparisonOp* aop;
    Operand* left;
    Operand* right;

    ~Cmp();
    Cmp();
    Cmp(string lhs, ComparisonOp* aop, Operand* left, Operand* right);
    string to_string() const;
    string get_type() const;
};

struct Copy : LirInst {
    string lhs;
    Operand* op;

    ~Copy();
    Copy();
    Copy(string lhs, Operand* op);
    string to_string() const;
    string get_type() const;
};

struct Gep : LirInst {
    string lhs;
    string src;
    Operand* idx;

    ~Gep();
    Gep();
    Gep(string lhs, string src, Operand* idx);
    string to_string() const;
    string get_type() const;
};

struct Gfp : LirInst {
    string lhs;
    string src;
    string field;

    Gfp();
    Gfp(string lhs, string src, string field);
    string to_string() const;
    string get_type() const;
};

struct Load : LirInst {
    string lhs;
    string src;

    Load();
    Load(string lhs, string src);
    string to_string() const;
    string get_type() const;
};

struct Store : LirInst {
    string dst;
    Operand* op;

    ~Store();
    Store();
    Store(string dst, Operand* op);
    string to_string() const;
    string get_type() const;
};

struct Branch : Terminal {
    Operand* guard;
    shared_ptr<Label> tt;
    shared_ptr<Label> ff;

    ~Branch();
    Branch();
    Branch(Operand* guard, shared_ptr<Label> tt, shared_ptr<Label> ff);
    string to_string() const;
    string get_type() const;
};

struct CallDirect : Terminal {
    string* lhs;
    string callee;
    vector<Operand*> args;
    string next_bb;

    ~CallDirect();
    CallDirect();
    CallDirect(string* lhs, string callee, vector<Operand*>& args, string next_bb);
    string to_string() const;
    string get_type() const;
};

struct CallIndirect : Terminal {
    string* lhs;
    string callee;
    vector<Operand*> args;
    string next_bb;

    ~CallIndirect();
    CallIndirect();
    CallIndirect(string* lhs, string callee, vector<Operand*>& args, string next_bb);
    string to_string() const;
    string get_type() const;
};

struct Jump : Terminal {
    shared_ptr<Label> next_bb;

    Jump();
    Jump(shared_ptr<Label> next_bb);
    string to_string() const;
    string get_type() const;
};

struct Ret : Terminal {
    Operand* op;

    ~Ret();
    Ret();
    Ret(Operand* op);
    string to_string() const;
    string get_type() const;
};

#endif 