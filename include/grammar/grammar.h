#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <memory>
#include "../ast_data_types/ast.h"

using namespace std;

shared_ptr<Type> typeg(vector<string>& tokens, int& index);
shared_ptr<Type> type_ad(vector<string>& tokens, int& index);
shared_ptr<FnType> type_op(vector<string>& tokens, int& index);
void type_fp(vector<string>& tokens, int& index, shared_ptr<FnType> f);
void type_ar(vector<string>& tokens, int& index, shared_ptr<FnType> f);
shared_ptr<FnType> funtype(vector<string>& tokens, int& index);
shared_ptr<Type> rettyp(vector<string>& tokens, int& index);
void program(vector<string>& tokens, int& index, Program& p);
void toplevel(vector<string>& tokens, int& index, Program& p);
void glob(vector<string>& tokens, int& index, Program& p);
void typdef(vector<string>& tokens, int& index, Program& p);
Decl* decl(vector<string>& tokens, int& index);
vector<Decl*> decls(vector<string>& tokens, int& index);
void exter(vector<string>& tokens, int& index, Program& p);
void fundef(vector<string>& tokens, int& index, Program& p);
void let(vector<string>& tokens, int& index, Function* f);
Stmt* stmt(vector<string>& tokens, int& index);
IfStmt* cond(vector<string>& tokens, int& index);
WhileStmt* loop(vector<string>& tokens, int& index);
vector<Stmt*> block(vector<string>& tokens, int& index);
Stmt* assign_or_call(vector<string>& tokens, int& index);
Stmt* gets_or_args(vector<string>& tokens, int& index, Lval* l);
Rhs* rhs(vector<string>& tokens, int& index);
Lval* lval(vector<string>& tokens, int& index);
void access(vector<string>& tokens, int& index, Lval*& l);
vector<Exp*> args(vector<string>& tokens, int& index);
Exp* exp(vector<string>& tokens, int& index);
Exp* exp_p4(vector<string>& tokens, int& index);
Exp* exp_p3(vector<string>& tokens, int& index);
Exp* exp_p2(vector<string>& tokens, int& index);
Exp* exp_p1(vector<string>& tokens, int& index);
void exp_ac(vector<string>& tokens, int& index, Exp*& e);
BinaryOp* binop_p1(vector<string>& tokens, int& index);
BinaryOp* binop_p2(vector<string>& tokens, int& index);
BinaryOp* binop_p3(vector<string>& tokens, int& index);

#endif