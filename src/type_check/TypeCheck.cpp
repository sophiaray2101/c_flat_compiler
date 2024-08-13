#include "../../include/type_check/TypeCheck.h"
#include "../../include/ast_data_types/Type.h"
#include <iostream>
#include <string>
#include <unordered_map>

Type* eTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Exp* e, Function* f) {
    if (e->get_type() == "NUM") {
        return NumTC();
    }
    else if (e->get_type() == "ID") {
        IdExp* i = dynamic_cast<IdExp*>(e);
        return IdTC(errors, G, i, f);
    }
    else if (e->get_type() == "NIL") {
        return NilTC();
    }
    else if (e->get_type() == "NEG") {
        UnOpExp* u = dynamic_cast<UnOpExp*>(e);
        return NegTC(errors, G, D, u, f);
    }
    else if (e->get_type() == "DEREF") {
        UnOpExp* u = dynamic_cast<UnOpExp*>(e);
        return DerefTC(errors, G, D, u, f);
    }
    else if (e->get_type() == "BINOP-EQ") {
        BinOpExp* b = dynamic_cast<BinOpExp*>(e);
        return BinopEqTC(errors, G, D, b, f);
    }
    else if (e->get_type() == "BINOP-REST") {
        BinOpExp* b = dynamic_cast<BinOpExp*>(e);
        return BinopRestTC(errors, G, D, b, f);
    }
    else if (e->get_type() == "ARRAY") {
        ArrayAccessExp* a = dynamic_cast<ArrayAccessExp*>(e);
        return ArrayTC(errors, G, D, a, f);
    }
    else if (e->get_type() == "FIELD") {
        FieldAccessExp* fa = dynamic_cast<FieldAccessExp*>(e); 
        return FieldTC(errors, G, D, fa, f);
    }
    else if (e->get_type() == "ECALL") {
        CallExp* c = dynamic_cast<CallExp*>(e);
        return ECallTC(errors, G, D, c, f);
    }
    else {
        return nullptr;
    }
}

Type* NumTC() {
    return new IntType();
}

Type* IdTC(vector<string>& errors, unordered_map<string, Type*>& G, IdExp* i, Function* f) {
    if (G.find(i->name) == G.end()) {
        errors.push_back("[ID] in function " + f->name + ": variable " + i->name + " undefined");
        G[i->name] = new AnyType();
    }
    return G[i->name];
}

Type* NilTC() {
    return new PtrType(make_shared<AnyType>());
}

Type* NegTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, UnOpExp* u, Function* f) {
    Type* n = eTC(errors, G, D, u->operand, f);
    string neg_type = n->get_type();
    if (neg_type != "int" && neg_type != "_") {
        errors.push_back("[NEG] in function " + f->name + ": negating type " + n->print_type() + " instead of int");
    }
    return new IntType();
}

Type* DerefTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, UnOpExp* u, Function* f) {
    Type* d = eTC(errors, G, D, u->operand, f);
    string deref_type = d->get_type();
    if (deref_type == "_") {
        return d;
    }
    else if (deref_type[0] != '&') {
        errors.push_back("[DEREF] in function " + f->name + ": dereferencing type " + d->print_type() + " instead of pointer");
        return new AnyType();
    }
    else {
        PtrType* p = dynamic_cast<PtrType*>(d);
        return p->ref.get();
    }
}

Type* BinopEqTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, BinOpExp* b, Function* f) {
    Type* l = eTC(errors, G, D, b->left, f);
    Type* r = eTC(errors, G, D, b->right, f);
    string left_type = l->get_type();
    string right_type = r->get_type();

    if (left_type != "int" && left_type != "_" && left_type[0] != '&') {
        errors.push_back("[BINOP-EQ] in function " + f->name + ": operand has non-primitive type " + l->print_type());
    }
    if (right_type != "int" && right_type != "_" && right_type[0] != '&') {
        errors.push_back("[BINOP-EQ] in function " + f->name + ": operand has non-primitive type " + r->print_type());
    }
    if (left_type != right_type && left_type != "_" && right_type != "_") {
        errors.push_back("[BINOP-EQ] in function " + f->name + ": operands with different types: " + l->print_type() + " vs " + r->print_type());
    }
    return new IntType();
}

Type* BinopRestTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, BinOpExp* b, Function* f) {
    Type* l = eTC(errors, G, D, b->left, f);
    Type* r = eTC(errors, G, D, b->right, f);
    string left_type = l->get_type();
    string right_type = r->get_type();
    
    if (left_type != "int" && left_type != "_") {
        errors.push_back("[BINOP-REST] in function " + f->name + ": operand has type " + l->print_type() + " instead of int");
    }
    if (right_type != "int" && right_type != "_") {
        errors.push_back("[BINOP-REST] in function " + f->name + ": operand has type " + r->print_type() + " instead of int");
    }
    return new IntType();
}

Type* ArrayTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, ArrayAccessExp* a, Function* f) {
    Type* p = eTC(errors, G, D, a->ptr, f);
    Type* i = eTC(errors, G, D, a->index, f);
    string ptr_type = p->get_type();
    string idx_type = i->get_type();

    if (idx_type != "_" && idx_type != "int") {
        errors.push_back("[ARRAY] in function " + f->name + ": array index is type " + i->print_type() + " instead of int");
    }

    if (ptr_type == "_") {
        return new AnyType();
    }
    else if (ptr_type[0] != '&') {
        errors.push_back("[ARRAY] in function " + f->name + ": dereferencing non-pointer type " + p->print_type());
        return new AnyType();
    }
    else {
        PtrType* temp = dynamic_cast<PtrType*>(p);
        return temp->ref.get();
    }
}

Type* FieldTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, FieldAccessExp* fa, Function* f) {
    Type* p = eTC(errors, G, D, fa->ptr, f);
    string ptr_type = p->get_type();
    string id;

    if (ptr_type == "_") {
        return new AnyType();
    }
    else if (ptr_type.length() < 7 || ptr_type.substr(0, 7) != "&struct") {
        errors.push_back("[FIELD] in function " + f->name + ": accessing field of incorrect type " + p->print_type());
        return new AnyType();
    }
    else {
        PtrType* temp = dynamic_cast<PtrType*>(p);
        Type* s = temp->ref.get();
        id = s->get_type();
        if (D.find(id) == D.end()) {
            errors.push_back("[FIELD] in function " + f->name + ": accessing field of non-existent struct type " + s->print_type());
            return new AnyType();
        }
        else {
            if (D[id].find(fa->field) == D[id].end()) {
                errors.push_back("[FIELD] in function " + f->name + ": accessing non-existent field " + fa->field + " of struct type " + s->print_type());
                return new AnyType();
            }
            else {
                return D[id][fa->field];
            }
        }
    }
}

Type* ECallTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallExp* c, Function* f) {
    IdExp* callee_name = dynamic_cast<IdExp*>(c->callee);
    if (callee_name && callee_name->name == "main") {
        errors.push_back("[ECALL-INTERNAL] in function " + f->name + ": calling main");

        if (G.find("main") != G.end() && G["main"]->get_type().substr(0,1) == "(") {
            FnType* ft = dynamic_cast<FnType*>(G["main"]);
            ECallExTC(errors, G, D, c, f, ft);
        }

        return new AnyType();
    }

    Type* call = eTC(errors, G, D, c->callee, f);
    string call_type = call->get_type();

    if (call_type == "_") {
        return new AnyType();
    }

    if (call && call_type[0] == '(') {
        FnType* ft = dynamic_cast<FnType*>(call);

        return ECallExTC(errors, G, D, c, f, ft);
    }
    else if (call && call_type.length() > 2 && call_type.substr(0, 2) == "&(") {
        PtrType* p = dynamic_cast<PtrType*>(call);
        FnType* ft = dynamic_cast<FnType*>(p->ref.get());
        return ECallInTC(errors, G, D, c, f, ft);
    }
    else {
        errors.push_back("[ECALL-*] in function " + f->name + ": calling non-function type " + call->print_type());
        return new AnyType();
    }
}

Type* ECallInTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallExp* c, Function* f, FnType* ft) {
    vector<Exp*> args = c->args;
    vector<shared_ptr<Type>> prms = ft->prms;
    
    if (args.size() != prms.size()) {
        errors.push_back("[ECALL-INTERNAL] in function " + f->name + ": call number of arguments (" + std::to_string(args.size()) + ") and parameters (" + std::to_string(prms.size()) + ") don't match");
    }

    for (int i = 0; i < min(args.size(), prms.size()); i++) {
        Type* arg = eTC(errors, G, D, args[i], f);
        Type* prm = prms[i].get();
        string arg_type = arg->get_type();
        string prm_type = prm->get_type();
        if (arg_type == "_" || prm_type == "_" || (arg_type == "&_" && prm_type[0] == '&') || (arg_type[0] == '_' && prm_type == "&_")) {
            continue;
        }
        if (arg_type != prm_type) {
            errors.push_back("[ECALL-INTERNAL] in function " + f->name + ": call argument has type " + arg->print_type() + " but parameter has type " + prm->print_type());
        }
    }
    if (!ft->ret) {
        errors.push_back("[ECALL-INTERNAL] in function " + f->name + ": calling a function with no return value");
        return new AnyType();
    }
    return ft->ret.get();
}

Type* ECallExTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallExp* c, Function* f, FnType* ft) {
    vector<Exp*> args = c->args;
    vector<shared_ptr<Type>> prms = ft->prms;
    
    if (args.size() != prms.size()) {
        errors.push_back("[ECALL-EXTERN] in function " + f->name + ": call number of arguments (" + std::to_string(args.size()) + ") and parameters (" + std::to_string(prms.size()) + ") don't match");
    }

    for (int i = 0; i < min(args.size(), prms.size()); i++) {
        Type* arg = eTC(errors, G, D, args[i], f);
        Type* prm = prms[i].get();
        string arg_type = arg->get_type();
        string prm_type = prm->get_type();
        if (arg_type == "_" || prm_type == "_" || (arg_type == "&_" && prm_type[0] == '&') || (arg_type[0] == '_' && prm_type == "&_")) {
            continue;
        }
        if (arg_type != prm_type) {
            errors.push_back("[ECALL-EXTERN] in function " + f->name + ": call argument has type " + arg->print_type() + " but parameter has type " + prm->print_type());
        }
    }

    if (!ft->ret) {
        errors.push_back("[ECALL-EXTERN] in function " + f->name + ": calling a function with no return value");
        return new AnyType();
    }
    return ft->ret.get();
}

void sTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Stmt* s, bool loop, Function* f) {
    if (s->get_type() == "BREAK") {
        BreakTC(errors, G, D, s, loop, f);
    }
    else if (s->get_type() == "CONTINUE") {
        ContinueTC(errors, G, D, s, loop, f);
    }
    else if (s->get_type() == "RETURN") {
        ReturnStmt* r = dynamic_cast<ReturnStmt*>(s);
        ReturnTC(errors, G, D, r, loop, f);
    }
    else if (s->get_type() == "ASSIGN") {
        AssignStmt* a = dynamic_cast<AssignStmt*>(s);
        AssignTC(errors, G, D, a, loop, f);
    }
    else if (s->get_type() == "SCALL") {
        CallStmt* c = dynamic_cast<CallStmt*>(s);
        SCallTC(errors, G, D, c, loop, f);
    }
    else if (s->get_type() == "IF") {
        IfStmt* i = dynamic_cast<IfStmt*>(s);
        IfTC(errors, G, D, i, loop, f);
    }
    else if (s->get_type() == "WHILE") {
        WhileStmt* w = dynamic_cast<WhileStmt*>(s);
        WhileTC(errors, G, D, w, loop, f);
    }
}

void BreakTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Stmt* s, bool loop, Function* f) {
    if (!loop) {
        errors.push_back("[BREAK] in function " + f->name + ": break outside of loop");
    }
}

void ContinueTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Stmt* s, bool loop, Function* f) {
    if (!loop) {
        errors.push_back("[CONTINUE] in function " + f->name + ": continue outside of loop");
    }
}

void ReturnTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, ReturnStmt* r, bool loop, Function* f) {
    Type* f_ret = f->rettyp.get();
    if (r->exp) { 
        Type* s_ret = eTC(errors, G, D, r->exp, f);
        string s_ret_type = s_ret->get_type();
        if (!f->rettyp) {
            errors.push_back("[RETURN-1] in function " + f->name + ": should return nothing but returning " + s_ret->print_type());
        }
        else {
            string f_ret_type = f_ret->get_type();
            if (f_ret_type == "_" || s_ret_type == "_" || (s_ret_type == "&_" && f_ret_type[0] == '&') || (s_ret_type[0] == '_' && f_ret_type == "&_")) {
                return;
            }
            if (s_ret_type != "_" && s_ret_type != f_ret_type) {
                errors.push_back("[RETURN-2] in function " + f->name + ": should return " + f_ret->print_type() + " but returning " + s_ret->print_type());
            }
        }
    }
    else {
        if (f->rettyp) {
            errors.push_back("[RETURN-2] in function " + f->name + ": should return " + f_ret->print_type() + " but returning nothing");
        }
    }
}

void AssignTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, AssignStmt* a, bool loop, Function* f) {
    Type* lval = LvalTC(errors, G, D, a->lhs, f);
    string lval_type = lval->get_type();

    if (a->rhs->get_type() == "EXP") {
        if ((lval_type.length() > 6 && lval_type.substr(0, 6) == "struct") || (lval_type.length() > 1 && lval_type[0] == '(')) {
            errors.push_back("[ASSIGN-EXP] in function " + f->name + ": assignment to struct or function");
        }

        RhsExp* rhs = dynamic_cast<RhsExp*>(a->rhs);
        Type* r = eTC(errors, G, D, rhs->exp, f);
        string r_type = r->get_type();

        if (r_type == "&_" && lval_type[0] == '&') {
            return;
        }  

        if (r_type != "_" && lval_type != "_" && lval_type != r_type) {
            errors.push_back("[ASSIGN-EXP] in function " + f->name + ": assignment lhs has type " + lval->print_type() + " but rhs has type " + r->print_type());
        }
    }
    if (a->rhs->get_type() == "NEW") {
        New* n = dynamic_cast<New*>(a->rhs);
        Type* t = n->type.get();
        Exp* a = n->amount;

        Type* amount = (!a) ? new IntType() : eTC(errors, G, D, a, f);
        string amount_type = amount->get_type();
        string type = t->get_type();
        
        if (amount_type != "_" && amount_type != "int") {
            errors.push_back("[ASSIGN-NEW] in function " + f->name + ": allocation amount is type " + amount->print_type() + " instead of int");
        }
       
        if (lval_type != "_" && type != "_" && lval_type.substr(1, lval_type.length() - 1) != type) {
            errors.push_back("[ASSIGN-NEW] in function " + f->name + ": assignment lhs has type " + lval->print_type() + " but we're allocating type " + t->print_type());
        }

        if (type[0] == '(') {
            errors.push_back("[ASSIGN-NEW] in function " + f->name + ": allocating function type " + t->print_type());
        }
    }

}

void SCallTC(vector<string>& errors,  unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallStmt* c, bool loop, Function* f) {
    IdLval* callee_name = dynamic_cast<IdLval*>(c->callee);
    if (callee_name && callee_name->name == "main") {
        errors.push_back("[SCALL-INTERNAL] in function " + f->name + ": calling main");

        if (G.find("&main") != G.end() && G["&main"]->get_type().substr(0, 2) == "&(") {
            PtrType* p = dynamic_cast<PtrType*>(G["&main"]);
            FnType* ft = dynamic_cast<FnType*>(p->ref.get());
        
            SCallInTC(errors, G, D, c, f, ft);
        }
        else if (G.find("main") != G.end() && G["main"]->get_type()[0] == '(') {
            FnType* ft = dynamic_cast<FnType*>(G["main"]);
            SCallExTC(errors, G, D, c, f, ft);
        }
        
        return;
    }
    
    Type* call = LvalTC(errors, G, D, c->callee, f);
    string call_type = call->get_type();

    if (call_type == "_") {
        return;
    }

    if (call && call_type[0] == '(') {
        FnType* ft = dynamic_cast<FnType*>(call);

        SCallExTC(errors, G, D, c, f, ft);
    }
    else if (call && call_type.length() > 2 && call_type.substr(0, 2) == "&(") {
        PtrType* p = dynamic_cast<PtrType*>(call);
        FnType* ft = dynamic_cast<FnType*>(p->ref.get());
        
        SCallInTC(errors, G, D, c, f, ft);
    }
    else {
        errors.push_back("[SCALL-*] in function " + f->name + ": calling non-function type " + call->print_type());
    }
}

void SCallInTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallStmt* c, Function* f, FnType* ft) {
    vector<Exp*> args = c->args;
    vector<shared_ptr<Type>> prms = ft->prms;
    
    if (args.size() != prms.size()) {
        errors.push_back("[SCALL-INTERNAL] in function " + f->name + ": call number of arguments (" + std::to_string(args.size()) + ") and parameters (" + std::to_string(prms.size()) + ") don't match");
    }

    for (int i = 0; i < min(args.size(), prms.size()); i++) {
        Type* arg = eTC(errors, G, D, args[i], f);
        Type* prm = prms[i].get();
        string arg_type = arg->get_type();
        string prm_type = prm->get_type();

        
        if (arg_type == "_" || prm_type == "_" || (arg_type == "&_" && prm_type[0] == '&') || (arg_type[0] == '_' && prm_type == "&_")) {
            continue;
        }
        if (arg_type != prm_type) {
            errors.push_back("[SCALL-INTERNAL] in function " + f->name + ": call argument has type " + arg->print_type() + " but parameter has type " + prm->print_type());
        }
    }
}

void SCallExTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, CallStmt* c, Function* f, FnType* ft) {
    vector<Exp*> args = c->args;
    vector<shared_ptr<Type>> prms = ft->prms;
    
    if (args.size() != prms.size()) {
        errors.push_back("[SCALL-EXTERN] in function " + f->name + ": call number of arguments (" + std::to_string(args.size()) + ") and parameters (" + std::to_string(prms.size()) + ") don't match");
    }

    for (int i = 0; i < min(args.size(), prms.size()); i++) {
        Type* arg = eTC(errors, G, D, args[i], f);
        Type* prm = prms[i].get();
        string arg_type = arg->get_type();
        string prm_type = prm->get_type();

        if (arg_type == "_" || prm_type == "_" || (arg_type == "&_" && prm_type[0] == '&') || (arg_type[0] == '_' && prm_type == "&_")) {
            continue;
        }
        if (arg_type != prm_type) {
            errors.push_back("[SCALL-EXTERN] in function " + f->name + ": call argument has type " + arg->print_type() + " but parameter has type " + prm->print_type());
        }
    }
}

void IfTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, IfStmt* i, bool loop, Function* f) {
    Type* guard = eTC(errors, G, D, i->guard, f);
    string guard_type = guard->get_type();

    if (guard_type != "_" && guard_type != "int") {
        errors.push_back("[IF] in function " + f->name + ": if guard has type " + guard->print_type() + " instead of int");
    }

    for (Stmt* s : i->tt) {
        sTC(errors, G, D, s, loop, f);
    }
    for (Stmt* s : i->ff) {
        sTC(errors, G, D, s, loop, f);
    }
} 

void WhileTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, WhileStmt* w, bool loop, Function* f) {
    Type* guard = eTC(errors, G, D, w->guard, f);
    string guard_type = guard->get_type();

    if (guard_type != "_" && guard_type != "int") {
        errors.push_back("[WHILE] in function " + f->name + ": while guard has type " + guard->print_type() + " instead of int");
    }

    for (Stmt* s : w->body) {
        sTC(errors, G, D, s, true, f);
    }
}

Type* LvalTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, Lval* l, Function* f) {
    if (l->get_type() == "ID") {
        IdLval* i = dynamic_cast<IdLval*>(l);
        return LvalIdTC(errors, G, i, f);
    }
    else if (l->get_type() == "DEREF") {
        DerefLval* d = dynamic_cast<DerefLval*>(l);
        return LvalDerefTC(errors, G, D, d, f);
    }
    else if (l->get_type() == "ARRAY") {
        ArrayAccessLval* a = dynamic_cast<ArrayAccessLval*>(l);
        return LvalArrayTC(errors, G, D, a, f);
    }
    else if (l->get_type() == "FIELD") {
        FieldAccessLval* fa = dynamic_cast<FieldAccessLval*>(l); 
        return LvalFieldTC(errors, G, D, fa, f);
    }
    else {
        return nullptr;
    }
}

Type* LvalIdTC(vector<string>& errors, unordered_map<string, Type*>& G, IdLval* i, Function* f) {
    if (G.find(i->name) == G.end()) {
        errors.push_back("[ID] in function " + f->name + ": variable " + i->name + " undefined");
        G[i->name] = new AnyType();
    }
    return G[i->name];
}

Type* LvalDerefTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, DerefLval* d, Function* f) {
    Type* deref = LvalTC(errors, G, D, d->lval, f);
    string deref_type = deref->get_type();
    if (deref_type == "_") {
        return new AnyType();
    }
    else if (deref_type[0] != '&') {
        errors.push_back("[DEREF] in function " + f->name + ": dereferencing type " + deref->print_type() + " instead of pointer");
        return new AnyType();
    }
    else {
        PtrType* p = dynamic_cast<PtrType*>(deref);
        return p->ref.get();
    }
}

Type* LvalArrayTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, ArrayAccessLval* a, Function* f) {
    Type* ptr = LvalTC(errors, G, D, a->ptr, f);
    Type* idx = eTC(errors, G, D, a->index, f);
    string ptr_type = ptr->get_type();
    string idx_type = idx->get_type();

    if (idx_type != "_" && idx_type != "int") {
        errors.push_back("[ARRAY] in function " + f->name + ": array index is type " + idx->print_type() + " instead of int");
    }

    if (ptr_type == "_") {
        return new AnyType();
    }
    else if (ptr_type[0] != '&') {
        errors.push_back("[ARRAY] in function " + f->name + ": dereferencing non-pointer type " + ptr->print_type());
        return new AnyType();
    }
    else {
        PtrType* temp = dynamic_cast<PtrType*>(ptr);
        return temp->ref.get();
    }
}

Type* LvalFieldTC(vector<string>& errors, unordered_map<string, Type*>& G, unordered_map<string, unordered_map<string, Type*>>& D, FieldAccessLval* fa, Function* f) {
    Type* ptr = LvalTC(errors, G, D, fa->ptr, f);
    string ptr_type = ptr->get_type();
    string id;

    if (ptr_type == "_") {
        return new AnyType();
    }
    else if (ptr_type.length() < 7 || ptr_type.substr(0, 7) != "&struct") {
        errors.push_back("[FIELD] in function " + f->name + ": accessing field of incorrect type " + ptr->print_type());
        return new AnyType();
    }
    else {
        PtrType* temp = dynamic_cast<PtrType*>(ptr);
        Type* s = temp->ref.get();
        id = s->get_type();
        if (D.find(id) == D.end()) {
            errors.push_back("[FIELD] in function " + f->name + ": accessing field of non-existent struct type " + s->print_type());
            return new AnyType();
        }
        else {
            if (D[id].find(fa->field) == D[id].end()) {
                errors.push_back("[FIELD] in function " + f->name + ": accessing non-existent field " + fa->field + " of struct type " + s->print_type());
                return new AnyType();
            }
            else {
                return D[id][fa->field];
            }
        }
    }

}

void ProgramTC(vector<string>& errors, Program& p) {
    unordered_map<string, Type*> G_0;
    unordered_map<string, unordered_map<string, Type*>> D;

    for (const Decl* d : p.globals) {
        string id = d->name;
        Type* type = d->type.get();
        G_0[id] = type;
    }

    for (const Struct* s : p.structs) {
        string id = s->name;
        vector<Decl*> fields = s->fields;
        for (Decl* d : fields) {
            D["struct_" + id][d->name] = d->type.get();
        }
    }

    for (const Decl* d : p.externs) {
        string id = d->name;
        Type* type = d->type.get();
        G_0[id] = type;
    }

    for (const Function* f : p.functions) {
        string id = f->name;

        vector<shared_ptr<Type>> prms;
        shared_ptr<Type> ret = f->rettyp;

        for (const Decl* d : f->params) {
            prms.push_back(d->type);
        }

        shared_ptr<FnType> func = make_shared<FnType>(prms, ret);
        Type* type = new PtrType(func);
        if (id == "main") {
            G_0["&main"] = type;
        }
        else {
            G_0[id] = type;
        }
    }

    for (Decl* d : p.globals) {
        GlobalTC(errors, G_0, D, d);
    }
    for (Struct* s : p.structs) {
        StructTC(errors, G_0, D, s);
    }
    for (Function* f : p.functions) {
        FunctionTC(errors, G_0, D, f);
    }
}

void GlobalTC(vector<string>& errors, unordered_map<string, Type*>& G_0, unordered_map<string, unordered_map<string, Type*>>& D, Decl* g) {
    string id = g->name;
    string type_id = g->type->get_type();
    if ((type_id.length() > 6 && type_id.substr(0, 6) == "struct") || (type_id.length() > 1 && type_id.substr(0, 1) == "(")) {
        errors.push_back("[GLOBAL] global " + id + " has a struct or function type");
    }
}

void StructTC(vector<string>& errors,  unordered_map<string, Type*>& G_0, unordered_map<string, unordered_map<string, Type*>>& D, Struct* s) {
    string id = s->name;
    for (Decl* d : s->fields) {
        string field = d->name;
        string type_id = d->type->get_type();
        if ((type_id.length() > 6 && type_id.substr(0, 6) == "struct") || (type_id.length() > 1 && type_id.substr(0, 1) == "(")) {
            errors.push_back("[STRUCT] struct " + id + " field " + field + " has a struct or function type");
        }
    }
}

void FunctionTC(vector<string>& errors,  unordered_map<string, Type*>& G_0, unordered_map<string, unordered_map<string, Type*>>& D, Function* f) {
    unordered_map<string, Type*> G_dub_prime = G_0;
    unordered_map<string, Type*> G_prime;
    for (Decl* d : f->params) {
        string id = d->name;
        Type* type = d->type.get();
        G_prime[id] = type;
    }
    for (pair<Decl*, Exp*> pair : f->locals) {
        string id = pair.first->name;
        Type* type = pair.first->type.get();
        G_prime[id] = type;
    }
    for (pair<string, Type*> pair : G_prime) {
        string id = pair.first;
        string type_id = pair.second->get_type();
        if ((type_id.length() > 6 && type_id.substr(0, 6) == "struct") || (type_id.length() > 1 && type_id.substr(0, 1) == "(")) {
            errors.push_back("[FUNCTION] in function " + f->name + ": variable " + id + " has a struct or function type");
        }
    }

    for (pair<string, Type*> pair : G_prime) {
        string id = pair.first;
        Type* type = pair.second;
        G_dub_prime[id] = type;
    }

    for (pair<Decl*, Exp*> pair : f->locals) {
        string var_name = pair.first->name;

        Type* left = pair.first->type.get();
        string left_type = left->get_type();

        Exp* e = pair.second;

        if (!e) continue;

        Type* right = eTC(errors, G_dub_prime, D, e, f);
        string right_type = right->get_type();

        if (right_type == "&_" && left_type[0] == '&') {
            continue;
        }  

        if (right_type != "_" && left_type != right_type) {
            errors.push_back("[FUNCTION] in function " + f->name + ": variable " + var_name + " with type " + left->print_type() + " has initializer of type " + right->print_type());
        }
    }

    for (Stmt* s : f->stmts) {
        sTC(errors, G_dub_prime, D, s, false, f);
    }
}