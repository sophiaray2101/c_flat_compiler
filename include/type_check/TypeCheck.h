#ifndef TYPECHECK_H
#define TYPECHECK_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../ast_data_types/ast.h"

using namespace std;

Type* eTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Exp* e, Function* f);
Type* NumTC();
Type* IdTC(vector<string>& errors, unordered_map<string, Type*>& G, IdExp* i, Function* f);
Type* NilTC();
Type* NegTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, UnOpExp* u, Function* f);
Type* DerefTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, UnOpExp* u, Function* f);
Type* BinopEqTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, BinOpExp* b, Function* f);
Type* BinopRestTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, BinOpExp* b, Function* f);
Type* ArrayTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, ArrayAccessExp* a, Function* f);
Type* FieldTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, FieldAccessExp* fa, Function* f);
Type* ECallTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallExp* c, Function* f);
Type* ECallInTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallExp* c, Function* f, FnType* ft);
Type* ECallExTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallExp* c, Function* f, FnType* ft);

void sTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Stmt* s, bool loop, Function* f);
void BreakTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Stmt* s, bool loop, Function* f);
void ContinueTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Stmt* s, bool loop, Function* f);
void ReturnTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, ReturnStmt* r, bool loop, Function* f);
void AssignTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, AssignStmt* a, bool loop, Function* f);
void SCallTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallStmt* c, bool loop, Function* f);
void SCallInTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallStmt* c, Function* f, FnType* ft);
void SCallExTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallStmt* c, Function* f, FnType* ft);
void IfTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, IfStmt* i, bool loop, Function* f);
void WhileTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, WhileStmt* w, bool loop, Function* f);

Type* LvalTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Lval* l, Function* f);
Type* LvalIdTC(vector<string>& errors, unordered_map<string, Type*>& G, IdLval* i, Function* f);
Type* LvalDerefTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, DerefLval* d, Function* f);
Type* LvalArrayTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, ArrayAccessLval* a, Function* f);
Type* LvalFieldTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, FieldAccessLval* fa, Function* f);

void ProgramTC(vector<string>& errors, Program& p);
void GlobalTC(vector<string>& errors, unordered_map<string, Type*>& G_0, unordered_map<string, unordered_map<string, Type*>>& D, Decl* g);
void StructTC(vector<string>& errors,  unordered_map<string, Type*>& G_0, unordered_map<string, unordered_map<string, Type*>>& D, Struct* s);
void FunctionTC(vector<string>& errors,  unordered_map<string, Type*>& G_0, unordered_map<string, unordered_map<string, Type*>>& D, Function* f);

#endif