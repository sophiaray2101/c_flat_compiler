#ifndef LOWERING_H
#define LOWERING_H

#include "../../include/ast_data_types/ast.h"
#include "../../include/lir_data_types/lir.h"
#include <map>
#include <memory>
#include <stack>
#include <unordered_set>

// ProgramLIR
struct ProgramLIR {
    map<string, shared_ptr<Type>> GlobalsLIR;
    map<string, map<string, shared_ptr<Type>>> StructsLIR;
    map<string, shared_ptr<Type>> ExternsLIR;
    map<string, FunctionLIR*> functionsLIR;

    vector<shared_ptr<LirInstructions>> translation;
    int lblCounter;
    int varCounter;

    int numRet;

    stack<string> while_hdrs;
    stack<string> while_ends;
    
    string lblIncrementer();
    string varIncrementer();
    AssignStmt* ConvertToAssign(Decl* d, Exp* e);
    shared_ptr<Type> findType(string id, FunctionLIR* flir);

    bool translate(Label* label, int& left, BasicBlockLIR* bb, string stopper, FunctionLIR* flir);
    bool conditional(Label* TT, int& left, BasicBlockLIR* bb, string stopper, FunctionLIR* flir);
    bool loop(Label* WHILE, int& left, BasicBlockLIR* bb, string stopper, FunctionLIR* flir);
    
    ProgramLIR(Program& programAST);
    
    void StmtLow(unordered_set<string>& locprm, Stmt* s, FunctionLIR* flir);
    void IfStmtLow(unordered_set<string>& locprm, IfStmt* i, FunctionLIR* flir);
    void WhileStmtLow(unordered_set<string>& locprm, WhileStmt* w, FunctionLIR* flir);
    void AssignStmtLow(unordered_set<string>& locprm, AssignStmt* a, FunctionLIR* flir);
    void CallStmtLow(unordered_set<string>& locprm, CallStmt* call, FunctionLIR* flir);
    void ContStmtLow();
    void BreakStmtLow();
    void RetStmtLow(unordered_set<string>& locprm, ReturnStmt* r, FunctionLIR* flir);

    Operand* ExpLow(unordered_set<string>& locprm, Exp* e, FunctionLIR* flir);
    Operand* NumExpLow(NumExp* num);
    Operand* IdExpLow(IdExp* i);
    Operand* NilExpLow();
    Operand* NegExpLow(unordered_set<string>& locprm, UnOpExp* neg, FunctionLIR* flir);
    Operand* DerefExpLow(unordered_set<string>& locprm, UnOpExp* deref, FunctionLIR* flir);
    Operand* BinOpRestExpLow(unordered_set<string>& locprm, BinOpExp* binoprest, FunctionLIR* flir);
    Operand* BinOpEqExpLow(unordered_set<string>& locprm, BinOpExp* binopeq, FunctionLIR* flir);
    Operand* ArrayAccExpLow(unordered_set<string>& locprm, ArrayAccessExp* arrayaccess, FunctionLIR* flir);
    Operand* FieldAccExpLow(unordered_set<string>& locprm, FieldAccessExp* fieldaccess, FunctionLIR* flir);
    Operand* CallExpLow(unordered_set<string>& locprm, CallExp* call, FunctionLIR* flir);

    Var* LvalLow(unordered_set<string>& locprm, Lval* l, FunctionLIR* flir);
    Var* LvalDerefLow(unordered_set<string>& locprm, DerefLval* deref, FunctionLIR* flir);
    Var* LvalArrayAccessLow(unordered_set<string>& locprm, ArrayAccessLval* arrayaccess, FunctionLIR* flir);
    Var* LvalFieldAccessLow(unordered_set<string>& locprm, FieldAccessLval* fieldaccess, FunctionLIR* flir);
    Var* LvalExpLow(unordered_set<string>& locprm, Lval* lv, FunctionLIR* flir);

    ~ProgramLIR();
    ProgramLIR();
    string to_string() const;
};

#endif