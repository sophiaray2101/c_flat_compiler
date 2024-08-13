#ifndef STMT_H
#define STMT_H

#include <string>
#include <vector>
#include "Exp.h"
#include "Lval.h"
#include "Rhs.h"

using namespace std;

// Stmt
struct Stmt {
    virtual ~Stmt() {}
    virtual string to_string() const = 0;
    virtual string get_type() const = 0;
};
struct BreakStmt : Stmt {
    string to_string() const override;
    string get_type() const override;
};
struct ContinueStmt : Stmt {
    string to_string() const override;
    string get_type() const override;
};
struct ReturnStmt : Stmt {
    Exp* exp;

    ReturnStmt(Exp* exp);
    ~ReturnStmt();
    string to_string() const override;
    string get_type() const override;
};
struct AssignStmt : Stmt {
    Lval* lhs;
    Rhs* rhs;

    AssignStmt(Lval* lhs, Rhs* rhs);
    ~AssignStmt();
    string to_string() const override;
    string get_type() const override;
};
struct CallStmt : Stmt {
    Lval* callee;
    vector<Exp*> args;

    CallStmt(Lval* callee, vector<Exp*>& args);
    ~CallStmt();
    string to_string() const override;
    string get_type() const override;
};
struct IfStmt : Stmt {
    Exp* guard;
    vector<Stmt*> tt;
    vector<Stmt*> ff;

    IfStmt(Exp* guard, vector<Stmt*>& tt, vector<Stmt*>& ff);
    ~IfStmt();
    string to_string() const override;
    string get_type() const override;
};
struct WhileStmt : Stmt {
    Exp* guard;
    vector<Stmt*> body;

    WhileStmt(Exp* guard, vector<Stmt*>& body);
    ~WhileStmt();
    string to_string() const override;
    string get_type() const override;
};

#endif