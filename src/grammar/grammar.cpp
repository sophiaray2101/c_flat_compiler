#include <iostream>
#include <vector>
#include <string>
#include "../../include/grammar/grammar.h"

using namespace std;

shared_ptr<Type> typeg(vector<string>& tokens, int& index) {
    int num_addresses = 0;
    while (index < tokens.size() && tokens[index] == "Address") {
        index++;
        num_addresses++;
    }

    auto t = type_ad(tokens, index);

    while (num_addresses > 0) {
        t = make_shared<PtrType>(t);
        num_addresses--;
    }
    return t;
}

shared_ptr<Type> type_ad(vector<string>& tokens, int& index) {
    if (tokens[index] == "Int") {
        index++;
        return make_shared<IntType>();
    }
    else if (tokens[index].length() >= 2 && tokens[index].substr(0,2) == "Id") {
        string name = tokens[index].substr(3, tokens[index].length() - 4);        
        index++;
        return make_shared<StructType>(name);
    }
    else if (tokens[index] == "OpenParen") {
        index++;
        return type_op(tokens, index);
    }
    else {
        throw index;
    }
    return nullptr;
}

shared_ptr<FnType> type_op(vector<string>& tokens, int& index) {
    auto f = make_shared<FnType>();
    if (tokens[index] == "CloseParen") {
        index++;
        type_ar(tokens, index, f);
    }
    else {
        f->prms.push_back(typeg(tokens, index));
        type_fp(tokens, index, f);
    }
    return f;
}

void type_fp(vector<string>& tokens, int& index, shared_ptr<FnType> f) {
    if (tokens[index] == "CloseParen") {
        index++;
        type_ar(tokens, index, f);
    }
    else if (tokens[index] == "Comma") {
        while (index < tokens.size() && tokens[index] == "Comma") {
            index++;
            f->prms.push_back(typeg(tokens, index));
        }

        if (tokens[index] != "CloseParen") {
            throw index;
        }
        index++;

        type_ar(tokens, index, f);
    }
    else {
        throw index;
    }
}

void type_ar(vector<string>& tokens, int& index, shared_ptr<FnType> f) {
    if (tokens[index] == "Arrow") {
        index++;
        f->ret = rettyp(tokens, index);
    }
    else {
        throw index;
    }
}

shared_ptr<FnType> funtype(vector<string>& tokens, int& index) {
    auto f = make_shared<FnType>();
    
    if (tokens[index] != "OpenParen") {
        throw index;
    }
    index++;

    if (index < tokens.size() && tokens[index] != "CloseParen") {
        f->prms.push_back(typeg(tokens, index));
        while (index < tokens.size() && tokens[index] == "Comma") {
            index++;
            f->prms.push_back(typeg(tokens, index));
        }
    }
    
    if (tokens[index] != "CloseParen") {
        throw index;
    }
    index++; 

    if (tokens[index] != "Arrow") {
        throw index;
    }
    index++; 

    f->ret = rettyp(tokens, index);

    return f;
}

shared_ptr<Type> rettyp(vector<string>& tokens, int& index) {
    if (tokens[index] == "Underscore") {
        index++;
        return nullptr;
    }
    else {
        return typeg(tokens, index);
    }
}

void program(vector<string>& tokens, int& index, Program& p) {
    if (index < tokens.size()) {
        toplevel(tokens, index, p);
        program(tokens, index, p);
    }
}

void toplevel(vector<string>& tokens, int& index, Program& p) {
    if (tokens[index] == "Let") {
        index++;
        glob(tokens, index, p);
    }
    else if (tokens[index] == "Struct") {
        index++;
        typdef(tokens, index, p);
    }
    else if (tokens[index] == "Extern") {
        index++;
        exter(tokens, index, p);
    }
    else if (tokens[index] == "Fn") {
        index++;
        fundef(tokens, index, p);
    }
    else {
        throw index;
    }
}

void glob(vector<string>& tokens, int& index, Program& p) {
    for (Decl* dec : decls(tokens, index)) {
        p.globals.push_back(dec);
    }

    if (tokens[index] != "Semicolon") {
        throw index;
    }
    index++;
}

void typdef(vector<string>& tokens, int& index, Program& p) {
    Struct* st = new Struct();

    if (tokens[index].length() < 2 || tokens[index].substr(0, 2) != "Id") {
        throw index;
    }
    st->name = tokens[index].substr(3, tokens[index].length() - 4);
    index++;

    if (tokens[index] != "OpenBrace") {
        throw index;
    }
    index++; 

    st->fields = decls(tokens, index);

    if (tokens[index] != "CloseBrace") {
        throw index;
    }
    index++;

    p.structs.push_back(st);
}

Decl* decl(vector<string>& tokens, int& index) {
    Decl* d = new Decl();
    if (tokens[index].length() < 2 || tokens[index].substr(0, 2) != "Id") {
        throw index;
    }
    d->name = tokens[index].substr(3, tokens[index].length() - 4);
    index++;

    if (tokens[index] != "Colon") {
        throw index;
    }
    index++; 

    d->type = typeg(tokens, index);
    return d;
}

vector<Decl*> decls(vector<string>& tokens, int& index) {
    vector<Decl*> d;
    d.push_back(decl(tokens, index));
    while (index < tokens.size() && tokens[index] == "Comma") {
        index++;
        d.push_back(decl(tokens, index));
    }
    return d;
}

void exter(vector<string>& tokens, int& index, Program& p) {
    Decl* d = new Decl();

    if (tokens[index].length() < 2 || tokens[index].substr(0, 2) != "Id") {
        throw index;
    }
    d->name = tokens[index].substr(3, tokens[index].length() - 4);
    index++; 

    if (tokens[index] != "Colon") {
        throw index;
    }
    index++;

    d->type = funtype(tokens, index);

    p.externs.push_back(d);

    if (tokens[index] != "Semicolon") {
        throw index;
    }
    index++;
}

void fundef(vector<string>& tokens, int& index, Program& p) {
    Function* f = new Function();

    if (tokens[index].length() < 2 || tokens[index].substr(0, 2) != "Id") {
        throw index;
    }
    f->name = tokens[index].substr(3, tokens[index].length() - 4);
    index++; 
    
    if (tokens[index] != "OpenParen") {
        throw index;
    }
    index++;

    if (index < tokens.size() && tokens[index] != "CloseParen") {
        f->params = decls(tokens, index);
    }

    if (tokens[index] != "CloseParen") {
        throw index;
    }
    index++; 

    if (tokens[index] != "Arrow") {
        throw index;
    }
    index++; 

    f->rettyp = rettyp(tokens, index);

    if (tokens[index] != "OpenBrace") {
        throw index;
    }
    index++; 
    
    while (index < tokens.size() && tokens[index] == "Let") {
        index++; 
        let(tokens, index, f);
    }

    if (tokens[index] == "CloseBrace") {
        throw index;
    }
    else {
        while (tokens[index] == "If" || tokens[index] == "While" || tokens[index] == "Star" || (tokens[index].length() > 2 && tokens[index].substr(0, 2) == "Id") || tokens[index] == "Break" || tokens[index] == "Continue" || tokens[index] == "Return") {
            f->stmts.push_back(stmt(tokens, index));
        }
    }

    if (tokens[index] != "CloseBrace") {
        throw index;
    }
    index++; 

    p.functions.push_back(f);
}

void let(vector<string>& tokens, int& index, Function* f) {
    Decl* d1 = decl(tokens, index);
    Exp* e1 = nullptr;

    if (index < tokens.size() && tokens[index] == "Gets") {
        index++;
        e1 = exp(tokens, index);
    }
    f->locals.push_back({d1, e1});

    while (index < tokens.size() && tokens[index] == "Comma") {
        index++;
        Decl* d_temp = decl(tokens, index);
        Exp* e_temp = nullptr;
        if (index < tokens.size() && tokens[index] == "Gets") {
            index++;
            e_temp = exp(tokens, index);
        }
        f->locals.push_back({d_temp, e_temp});
    }

    if (tokens[index] != "Semicolon") {
        throw index;
    }
    index++;
}

Stmt* stmt(vector<string>& tokens, int& index) {
    if (tokens[index] == "If") {
        index++;
        return cond(tokens, index);
    } 
    else if (tokens[index] == "While") {
        index++;
        return loop(tokens, index);
    } 
    else if (tokens[index] == "Star" || (tokens[index].length() > 2 && tokens[index].substr(0, 2) == "Id")) {
        Stmt* s = assign_or_call(tokens, index);

        if (tokens[index] != "Semicolon") {
            throw index;
        }
        index++; 

        return s;
    } 
    else if (tokens[index] == "Break") {
        if (tokens[index] != "Break") {
            throw index;
        }
        index++;

        if (tokens[index] != "Semicolon") {
            throw index;
        }
        index++;

        return new BreakStmt();
    } 
    else if (tokens[index] == "Continue") {
        if (tokens[index] != "Continue") {
            throw index;
        }
        index++; 

        if (tokens[index] != "Semicolon") {
            throw index;
        }
        index++; 

        return new ContinueStmt();
    } 
    else if (tokens[index] == "Return") {
        index++; 
        Exp* e = nullptr;
        if (index < tokens.size() && tokens[index] != "Semicolon") {
            e = exp(tokens, index);
        }

        if (tokens[index] != "Semicolon") {
            throw index;
        }
        index++;

        return new ReturnStmt(e);
    }
    else {
        throw index;
    }
    return nullptr;
}

IfStmt* cond(vector<string>& tokens, int& index) {
    Exp* guard = exp(tokens, index);
    vector<Stmt*> tt = block(tokens, index);
    vector<Stmt*> ff;
    if (index < tokens.size() && tokens[index] == "Else") {
        index++;
        ff = block(tokens, index);
    }
    return new IfStmt(guard, tt, ff);
}

WhileStmt* loop(vector<string>& tokens, int& index) {
    Exp* guard = exp(tokens, index);
    vector<Stmt*> body = block(tokens, index);
    return new WhileStmt(guard, body);
}

vector<Stmt*> block(vector<string>& tokens, int& index) {
    if (tokens[index] != "OpenBrace") {
        throw index;
    }
    index++; 

    vector<Stmt*> ret;
    while (tokens[index] == "If" || tokens[index] == "While" || tokens[index] == "Star" || (tokens[index].length() > 2 && tokens[index].substr(0, 2) == "Id") || tokens[index] == "Break" || tokens[index] == "Continue" || tokens[index] == "Return") {
        ret.push_back(stmt(tokens, index));
    }

    if (tokens[index] != "CloseBrace") {
        throw index;
    }
    index++;

    return ret;
}

Stmt* assign_or_call(vector<string>& tokens, int& index) {
    Lval* l = lval(tokens, index);
    return gets_or_args(tokens, index, l);
}

Stmt* gets_or_args(vector<string>& tokens, int& index, Lval* l) {
    if (tokens[index] == "Gets") {
        index++;
        Rhs* r = rhs(tokens, index);
        return new AssignStmt(l, r);
    }
    else if (tokens[index] == "OpenParen") {
        index++;
        vector<Exp*> a;
        if (index < tokens.size() && tokens[index] != "CloseParen") {
            a = args(tokens, index);
        }

        if (tokens[index] != "CloseParen") {
            throw index;
        }
        index++; 

        return new CallStmt(l, a);
    }
    else {
        throw index;
    }
    return nullptr;
}

Rhs* rhs(vector<string>& tokens, int& index) {
    if (index < tokens.size() && tokens[index] == "New") {
        index++;
        shared_ptr<Type> t = typeg(tokens, index);
        Exp* e = nullptr;
        if (index < tokens.size() && tokens[index] != "Semicolon") {
            e = exp(tokens, index);
        }
        return new New(t, e);
    }
    else {
        return new RhsExp(exp(tokens, index));
    }
}

Lval* lval(vector<string>& tokens, int& index) {
    int num_stars = 0;
    while (index < tokens.size() && tokens[index] == "Star") {
        num_stars++;
        index++;
    }

    if (tokens[index].length() < 2 || tokens[index].substr(0, 2) != "Id") {
        throw index;
    }
    Lval* l = new IdLval(tokens[index].substr(3, tokens[index].length() - 4));
    index++;

    while (index < tokens.size() && (tokens[index] == "OpenBracket" || tokens[index] == "Dot")) {
        access(tokens, index, l);
    }

    while (num_stars > 0) {
        l = new DerefLval(l);
        num_stars--;
    }

    return l;
}

void access(vector<string>& tokens, int& index, Lval*& l) {
    if (tokens[index] == "OpenBracket") {
        index++;
        Exp* e = exp(tokens, index);
        l = new ArrayAccessLval(l, e);

        if (tokens[index] != "CloseBracket") {
            throw index;
        }
        index++; 
    }
    else if (tokens[index] == "Dot") {
        index++;
        
        if (tokens[index].length() < 2 || tokens[index].substr(0, 2) != "Id") {
            throw index;
        }   
        string name = tokens[index].substr(3, tokens[index].length() - 4);
        index++;

        l = new FieldAccessLval(l, name);
    }
    else {
        throw index;
    }
}

vector<Exp*> args(vector<string>& tokens, int& index) {
    vector<Exp*> a;
    a.push_back(exp(tokens, index));
    while (index < tokens.size() && tokens[index] == "Comma") {
        index++;
        a.push_back(exp(tokens, index));
    }
    return a;
}

Exp* exp(vector<string>& tokens, int& index) {
    Exp* e1 = exp_p4(tokens, index);
    
    while (index < tokens.size() && (tokens[index] == "Equal" || tokens[index] == "NotEq" || tokens[index] == "Lt" || tokens[index] == "Lte" || tokens[index] == "Gt" || tokens[index] == "Gte")) {
        BinaryOp* b = binop_p3(tokens, index);
        Exp* e2 = exp_p4(tokens, index);
        e1 = new BinOpExp(b, e1, e2);
    }

    return e1;
}

Exp* exp_p4(vector<string>& tokens, int& index) {
    Exp* e1 = exp_p3(tokens, index);
      
    while (index < tokens.size() && (tokens[index] == "Plus" || tokens[index] == "Dash")) {
        BinaryOp* b = binop_p2(tokens, index);
        Exp* e2 = exp_p3(tokens, index);
        e1 = new BinOpExp(b, e1, e2);
    }
        
    return e1;
}

Exp* exp_p3(vector<string>& tokens, int& index) {
    Exp* e1 = exp_p2(tokens, index);
    
    while (index < tokens.size() && (tokens[index] == "Star" || tokens[index] == "Slash")) {
        BinaryOp* b = binop_p1(tokens, index);
        Exp* e2 = exp_p2(tokens, index);
        e1 = new BinOpExp(b, e1, e2);
    }

    return e1;
}

Exp* exp_p2(vector<string>& tokens, int& index) {
    vector<string> unops;
    while (index < tokens.size() && (tokens[index] == "Star" || tokens[index] == "Dash")) {
        unops.push_back(tokens[index]);
        index++;
    }

    Exp* e = exp_p1(tokens, index);

    for (int i = unops.size() - 1; i >= 0; i--) {
        if (unops[i] == "Star") {
            e = new UnOpExp(new Deref(), e); 
        }
        else if (unops[i] == "Dash") {
            e = new UnOpExp(new Neg(), e);
        }
    }

    return e;
}

Exp* exp_p1(vector<string>& tokens, int& index) {
    if (index < tokens.size()) {
        if (tokens[index].substr(0, 3) == "Num") {
            uint32_t num = stoi(tokens[index].substr(4, tokens[index].length() - 5));
            index++;
            return new NumExp(num);
        }
        else if (tokens[index] == "Nil") {
            index++;
            return new NilExp();
        }
        else if (tokens[index] == "OpenParen") {
            index++;
            Exp* ret = exp(tokens, index);

            if (tokens[index] != "CloseParen") {
                throw index;
            }
            index++; 

            return ret;
        }
        else if (tokens[index].substr(0, 2) == "Id") {
            Exp* ret = new IdExp(tokens[index].substr(3, tokens[index].length() - 4));
            index++;

            while (tokens[index] == "OpenBracket" || tokens[index] == "Dot" || tokens[index] == "OpenParen") {
                exp_ac(tokens, index, ret);
            }
            return ret;
        }
        else {
            throw index;
        }
    }
    return nullptr;
}

void exp_ac(vector<string>& tokens, int& index, Exp*& e) {
    if (index < tokens.size()) {
        if (tokens[index] == "OpenBracket") {
            index++;
            e = new ArrayAccessExp(e, exp(tokens, index));

            if (tokens[index] != "CloseBracket") {
                throw index;
            }
            index++;
        }
        else if (tokens[index] == "Dot") {
            index++;

            if (tokens[index].length() < 2 || tokens[index].substr(0, 2) != "Id") {
                throw index;
            }  
            string name = tokens[index].substr(3, tokens[index].length() - 4);
            index++; 

            e = new FieldAccessExp(e, name);
        }
        else if (tokens[index] == "OpenParen") {
            index++;
            vector<Exp*> arg;
            if (index < tokens.size() && tokens[index] != "CloseParen") {
                arg = args(tokens, index);
            }
            e = new CallExp(e, arg);

            if (tokens[index] != "CloseParen") {
                throw index;
            }
            index++;
        }
        else {
            throw index;
        }
    }
}

BinaryOp* binop_p1(vector<string>& tokens, int& index) {
    if (tokens[index] == "Star") {
        index++;
        return new Mul();
    }
    else if (tokens[index] == "Slash") {
        index++;
        return new Div();
    }
    else {
        throw index;
    }
    return nullptr;
}

BinaryOp* binop_p2(vector<string>& tokens, int& index) {
    if (tokens[index] == "Plus") {
        index++;
        return new Add();
    }
    else if (tokens[index] == "Dash") {
        index++;
        return new Sub();
    }
    else {
        throw index;
    }
    return nullptr;
}

BinaryOp* binop_p3(vector<string>& tokens, int& index) {
    if (tokens[index] == "Equal") {
        index++;
        return new Equal();
    }
    else if (tokens[index] == "NotEq") {
        index++;
        return new NotEq();
    }
    else if (tokens[index] == "Lt") {
        index++;
        return new Lt();
    }
    else if (tokens[index] == "Lte") {
        index++;
        return new Lte();
    }
    else if (tokens[index] == "Gt") {
        index++;
        return new Gt();
    }
    else if (tokens[index] == "Gte") {
        index++;
        return new Gte();
    }
    else {
        throw index;
    }
    return nullptr;
}