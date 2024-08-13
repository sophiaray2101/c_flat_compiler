#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory>
#include "Type.h"
#include "Exp.h"
#include "Stmt.h"

using namespace std;

// AST
struct Decl {
    string name;
    shared_ptr<Type> type;

    Decl();
    Decl(string name, shared_ptr<Type> type);
    ~Decl();
    string to_string() const;
};

struct Struct {
    string name;
    vector<Decl*> fields;

    Struct();
    Struct(string name, vector<Decl*>& fields);
    ~Struct();
    string to_string() const;
};

struct Function {
    string name;
    vector<Decl*> params;
    shared_ptr<Type> rettyp;
    vector<pair<Decl*, Exp*>> locals;
    vector<Stmt*> stmts;

    Function();
    Function(string name, vector<Decl*>& params, shared_ptr<Type> rettyp, vector<pair<Decl*, Exp*>>& locals, vector<Stmt*>& stmts);
    ~Function();
    string to_string() const;
};

struct Program {
    vector<Decl*> globals;
    vector<Struct*> structs;
    vector<Decl*> externs;
    vector<Function*> functions;

    Program();
    Program(vector<Decl*>& globals, vector<Struct*> structs, vector<Decl*>& externs, vector<Function*>& functions);
    ~Program();
    string to_string() const;
};

#endif 