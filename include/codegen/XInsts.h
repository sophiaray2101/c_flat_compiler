#ifndef XINSTS_H
#define XINSTS_H

#include <string>
#include <memory>
#include "XOps.h"

using namespace std;

// XInsts
struct XInsts {
    virtual ~XInsts() {}
    virtual string to_string() const = 0;
};

struct movq : XInsts {
    shared_ptr<XOps> src;
    shared_ptr<XOps> dest;

    movq(shared_ptr<XOps> src, shared_ptr<XOps> dest);
    string to_string() const;
};

struct leaq : XInsts {
    shared_ptr<XOps> src;
    shared_ptr<XOps> dest;

    leaq(shared_ptr<XOps> src, shared_ptr<XOps> dest);
    string to_string() const;
};

struct lea : XInsts {
    shared_ptr<XOps> src;
    shared_ptr<XOps> dest;

    lea(shared_ptr<XOps> src, shared_ptr<XOps> dest);
    string to_string() const;
};

struct pushq : XInsts {
    shared_ptr<XOps> src;

    pushq(shared_ptr<XOps> src);
    string to_string() const;
};

struct popq : XInsts {
    shared_ptr<XOps> dest;

    popq(shared_ptr<XOps> dest);
    string to_string() const;
};

struct incq : XInsts {
    shared_ptr<XOps> dest;

    incq(shared_ptr<XOps> dest);
    string to_string() const;
};

struct decq : XInsts {
    shared_ptr<XOps> dest;

    decq(shared_ptr<XOps> dest);
    string to_string() const;
};

struct addq : XInsts {
    shared_ptr<XOps> src;
    shared_ptr<XOps> dest;

    addq(shared_ptr<XOps> src, shared_ptr<XOps> dest);
    string to_string() const;
};

struct subq : XInsts {
    shared_ptr<XOps> src;
    shared_ptr<XOps> dest;

    subq(shared_ptr<XOps> src, shared_ptr<XOps> dest);
    string to_string() const;
};

struct imulq : XInsts {
    shared_ptr<XOps> src;
    shared_ptr<XOps> dest;

    imulq(shared_ptr<XOps> src, shared_ptr<XOps> dest);
    string to_string() const;
};

struct idivq : XInsts {
    shared_ptr<XOps> src;

    idivq(shared_ptr<XOps> src);
    string to_string() const;
};

struct cmpq : XInsts {
    shared_ptr<XOps> src2;
    shared_ptr<XOps> src1;

    cmpq(shared_ptr<XOps> src2, shared_ptr<XOps> src1);
    string to_string() const;
};

struct setX : XInsts{
    shared_ptr<Reg> dest;
    string cmpType;
   
    setX(shared_ptr<Reg> dest, string cmpType);
    string to_string() const;
};

struct jmp : XInsts {
    string lbl;

    jmp(string lbl);
    string to_string() const;
};

struct jX : XInsts {
    string cmpType;
    string lbl;

    jX(string cmpType, string lbl);
    string to_string() const;
};

struct call : XInsts {
    string lbl;

    call(string lbl);
    string to_string() const;
};

struct ret : XInsts {
    string to_string() const;
};

struct cqo : XInsts {
    string to_string() const;
};

#endif