#include "../../include/ast_data_types/ast.h"
#include <iostream>
using namespace std;

Decl::Decl() {}
Decl::Decl(string name, shared_ptr<Type> type) : name(name), type(type) {}
Decl::~Decl() {
}
string Decl::to_string() const {
    return "Decl(" + this->name + ", " + this->type->to_string() + ")";
}

Struct::Struct() {}
Struct::Struct(string name, vector<Decl*>& fields) : name(name), fields(fields) {}
Struct::~Struct() {
    for (auto decl : this->fields) {
        delete decl;
        decl = nullptr;
    }
}
string Struct::to_string() const {
    string val = "Struct(\n  name = " + this->name + ",\n  fields = ["; 
    bool first = true;
    for (const auto& field : this->fields) {
        if (!first) {
            val += ",\n    ";
        }
        val += field->to_string();
        first = false;
    }
    val += "\n  ]\n)";
    return val;
}

Function::Function() {}
Function::Function(string name, vector<Decl*>& params, shared_ptr<Type> rettyp, vector<pair<Decl*, Exp*>>& locals, vector<Stmt*>& stmts) : name(name), params(params), rettyp(rettyp), locals(locals), stmts(stmts) {}
Function::~Function() {
    for (auto prm : this->params) {
        delete prm;
        prm = nullptr;
    }
    for (auto loc : this->locals) {
        delete loc.first;
        loc.first = nullptr;
        delete loc.second;
        loc.second = nullptr;
    }
    for (auto stmt : this->stmts) {
        delete stmt;
        stmt = nullptr;
    }
}
string Function::to_string() const {
    string val = "Function(\n  name = " + this->name + ",\n  params = [";
    bool first = true;
    for (const auto& field : this->params) {
        if (!first) {
            val += ",\n    ";
        }
        val += field->to_string();
        first = false;
    }
    val += "\n  ],\n  rettyp = ";
    val += (!this->rettyp) ? "_" : this->rettyp->to_string();
    val += ",\n  locals = [\n    ";
    first = true;
    for (const auto& loc : this->locals) {
        if (!first) {
            val += ",\n    ";
        }
        val += "(" + loc.first->to_string() + ", ";
        val += (!loc.second) ? "_" : loc.second->to_string();
        val += ")";
        first = false;
    }
    val += "\n  ],\n  stmts = [";
    first = true;
    for (const auto& stmt : this->stmts) {
        if (!first) {
            val += ",\n    ";
        }
        val += stmt->to_string();
        first = false;
    }
    val += "\n  ]\n)";
    return val;
}

Program::Program() {}
Program::Program(vector<Decl*>& globals, vector<Struct*> structs, vector<Decl*>& externs, vector<Function*>& functions) : globals(globals), structs(structs), externs(externs), functions(functions) {}
Program::~Program() {
    for (auto glob : this->globals) {
        delete glob;
        glob = nullptr;
    }
    for (auto struc : this->structs) {
        delete struc; 
        struc  = nullptr;
    }
    for (auto ext : this->externs) {
        delete ext;
        ext = nullptr;
    }
    for (auto funct : this->functions) {
        delete funct;
        funct = nullptr;
    }
}
string Program::to_string() const {
    string val = "Program(\n  globals = [\n    ";
    bool first = true;
    for (const auto& glob : this->globals) {
        if (!first) {
            val += ",\n    ";
        }
        val += glob->to_string();
        first = false;
    }
    val += "\n  ],\n  structs = [\n    ";
    first = true;
    for (const auto& struc : this->structs) {
        if (!first) {
            val += ",\n    ";
        }
        val += struc->to_string();
        first = false;
    }
    val += "\n  ],\n  externs = [";
    first = true;
    for (const auto& ext : this->externs) {
        if (!first) {
            val += ",\n    ";
        }
        val += ext->to_string();
        first = false;
    }
    val += "\n  ],\n  functions = [\n    ";
    first = true;
    for (const auto& funct : this->functions) {
        if (!first) {
            val += ",\n    ";
        }
        val += funct->to_string();
        first = false;
    }
    val += "\n  ]\n)";
    return val;
}