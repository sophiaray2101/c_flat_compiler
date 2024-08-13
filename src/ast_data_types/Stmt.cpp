#include "../../include/ast_data_types/Stmt.h"

// Stmt
string BreakStmt::to_string() const {
    return "Break";
}
string BreakStmt::get_type() const {
    return "BREAK";
}

string ContinueStmt::to_string() const {
    return "Continue";
}
string ContinueStmt::get_type() const {
    return "CONTINUE";
}

ReturnStmt::ReturnStmt(Exp* exp) : exp(exp) {}
ReturnStmt::~ReturnStmt() {
    delete this->exp;
}
string ReturnStmt::to_string() const {
    string val = (!this->exp) ? "_" : this->exp->to_string();
    return "Return(" + val + ")";
}
string ReturnStmt::get_type() const {
    return "RETURN";
}

AssignStmt::AssignStmt(Lval* lhs, Rhs* rhs) : lhs(lhs), rhs(rhs) {}
AssignStmt::~AssignStmt() {
    delete this->lhs;
    delete this->rhs;
}
string AssignStmt::to_string() const {
    return "Assign(\n  lhs = " + this->lhs->to_string() + ",\n  rhs = " + this->rhs->to_string() + "\n)";
}
string AssignStmt::get_type() const {
    return "ASSIGN";
}

CallStmt::CallStmt(Lval* callee, vector<Exp*>& args) : callee(callee), args(args) {}
CallStmt::~CallStmt() {
    delete this->callee;
    for (auto exp : this->args) {
        delete exp;
    } 
}
string CallStmt::to_string() const {
    string val = "Call(\n  callee = " + this->callee->to_string() + ",\n  args = [\n    ";
    bool first = true;
    for (const auto& exp : this->args) {
        if (!first) {
            val += ",\n    ";
        }
        val += exp->to_string();
        first = false;
    }
    val += "\n  ]\n)";
    return val;
}
string CallStmt::get_type() const {
    return "SCALL";
}

IfStmt::IfStmt(Exp* guard, vector<Stmt*>& tt, vector<Stmt*>& ff) : guard(guard), tt(tt), ff(ff) {}
IfStmt::~IfStmt() {
    delete this->guard;
    for (auto stmt : tt) {
        delete stmt;
    }
    for (auto stmt : ff) {
        delete stmt;
    }
}
string IfStmt::to_string() const {
    string val = "If(\n  guard = " + this->guard->to_string() + ",\n  tt = [\n    ";
    bool first = true;
    for (const auto& stmt : this->tt) {
        if (!first) {
            val += ",\n    ";
        }
        val += stmt->to_string();
        first = false;
    }
    val += "\n  ],\n  ff = [\n    ";
    first = true;
    for (const auto& stmt : this->ff) {
        if (!first) {
            val += ",\n    ";
        }
        val += stmt->to_string();
        first = false;
    }
    val += "\n  ]\n)";
    return val;
}
string IfStmt::get_type() const {
    return "IF";
}

WhileStmt::WhileStmt(Exp* guard, vector<Stmt*>& body) : guard(guard), body(body) {}
WhileStmt::~WhileStmt() {
    delete this->guard;
    for (auto stmt : body) {
        delete stmt;
    }
}
string WhileStmt::to_string() const {
    string val = "While(\n  guard = " + this->guard->to_string() + ",\n  body = [\n    ";
    bool first = true;
    for (const auto& stmt : this->body) {
        if (!first) {
            val += ",\n    ";
        }
        val += stmt->to_string();
        first = false;
    }
    val += "\n  ]\n)";
    return val;
}
string WhileStmt::get_type() const {
    return "WHILE";
}