#include "../../include/codegen/XInsts.h"

movq::movq(shared_ptr<XOps> src, shared_ptr<XOps> dest) : src(src), dest(dest) {}
string movq::to_string() const {
    return "movq " + this->src->to_string() + ", " + this->dest->to_string();
}

leaq::leaq(shared_ptr<XOps> src, shared_ptr<XOps> dest) : src(src), dest(dest) {}
string leaq::to_string() const {
    return "leaq " + this->src->to_string() + ", " + this->dest->to_string();
}

lea::lea(shared_ptr<XOps> src, shared_ptr<XOps> dest) : src(src), dest(dest) {}
string lea::to_string() const {
    return "lea " + this->src->to_string() + ", " + this->dest->to_string();
}

pushq::pushq(shared_ptr<XOps> src) : src(src) {}
string pushq::to_string() const {
    return "pushq " + this->src->to_string();
}

popq::popq(shared_ptr<XOps> dest) : dest(dest) {}
string popq::to_string() const {
    return "popq " + this->dest->to_string();
}

incq::incq(shared_ptr<XOps> dest) : dest(dest) {}
string incq::to_string() const {
    return "incq " + this->dest->to_string();
}

decq::decq(shared_ptr<XOps> dest) : dest(dest) {}
string decq::to_string() const {
    return "decq " + this->dest->to_string();
}

addq::addq(shared_ptr<XOps> src, shared_ptr<XOps> dest) : src(src), dest(dest) {}
string addq::to_string() const {
    return "addq " + this->src->to_string() + ", " + this->dest->to_string();
}

subq::subq(shared_ptr<XOps> src, shared_ptr<XOps> dest) : src(src), dest(dest) {}
string subq::to_string() const {
    return "subq " + this->src->to_string() + ", " + this->dest->to_string();
}

imulq::imulq(shared_ptr<XOps> src, shared_ptr<XOps> dest) : src(src), dest(dest) {}
string imulq::to_string() const {
    return "imulq " + this->src->to_string() + ", " + this->dest->to_string();
}

idivq::idivq(shared_ptr<XOps> src) : src(src) {}
string idivq::to_string() const {
    return "idivq " + this->src->to_string();
}

cmpq::cmpq(shared_ptr<XOps> src2, shared_ptr<XOps> src1) : src2(src2), src1(src1) {}
string cmpq::to_string() const {
    return "cmpq " + this->src2->to_string() + ", " + this->src1->to_string();
}

setX::setX(shared_ptr<Reg> dest, string cmpType) : dest(dest), cmpType(cmpType) {}
string setX::to_string() const{
    return "set" + this->cmpType + " " + this->dest->to_string();
}

jmp::jmp(string lbl) : lbl(lbl) {}
string jmp::to_string() const {
    return "jmp " + this->lbl;
}

jX::jX(string cmpType, string lbl) : cmpType(cmpType), lbl(lbl) {}
string jX::to_string() const {
    return "j" + this->cmpType + " " + this->lbl;
}

call::call(string lbl) : lbl(lbl) {}
string call::to_string() const {
    return "call " + this->lbl;
}

string ret::to_string() const {
    return "ret";
}

string cqo::to_string() const {
    return "cqo";
}