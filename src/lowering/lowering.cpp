#include "../../include/lowering/lowering.h"
#include "../../include/ast_data_types/ast.h"
#include "../../include/lir_data_types/lir.h"
#include <string>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

using namespace std;

string ProgramLIR::lblIncrementer() {
    return "lbl" + std::to_string(this->lblCounter++);
}

string ProgramLIR::varIncrementer() {
    return "_t" + std::to_string(this->varCounter++);
}

AssignStmt* ProgramLIR::ConvertToAssign(Decl* d, Exp* e) {
    Lval* lhs = new IdLval(d->name);
    Rhs* rhs = new RhsExp(e);
    return new AssignStmt(lhs, rhs);
}

shared_ptr<Type> ProgramLIR::findType(string id, FunctionLIR* flir) {
    if (flir->locals.find(id) != flir->locals.end()) {
        return flir->locals[id];
    }
    for (pair<string, shared_ptr<Type>> prm : flir->params) {
        if (prm.first == id) {
            return prm.second;
        }
    }
    return this->GlobalsLIR[id];
}

ProgramLIR::ProgramLIR(Program& ProgramAST) {
    ProgramLIR* programLIR = new ProgramLIR();

    for (Decl* d : ProgramAST.globals) {
        this->GlobalsLIR[d->name] = d->type;
    }

    for (Struct* s : ProgramAST.structs) {
        map<string, shared_ptr<Type>> structMap;
        for (Decl* d : s->fields) {
            structMap[d->name] = d->type;
        }
        this->StructsLIR[s->name] = structMap;
    }

    for (Decl* d : ProgramAST.externs) {
        this->ExternsLIR[d->name] = d->type;
    }

    for (Function* f : ProgramAST.functions) {
        if (f->name == "main") {
            continue;
        }

        vector<shared_ptr<Type>> prms;
        shared_ptr<Type> ret = f->rettyp;

        for (Decl* prm : f->params) {
            prms.push_back(prm->type);
        }

        this->GlobalsLIR[f->name] = make_shared<PtrType>(make_shared<FnType>(prms, ret));
    }

    for (Function* f : ProgramAST.functions) {
        this->translation = {make_shared<Label>("entry", "entry")};

        FunctionLIR* flir = new FunctionLIR();
        flir->name = f->name;
        flir->rettyp = f->rettyp;
        for (Decl* prm : f->params)  {
            flir->params.push_back({prm->name, prm->type});
        }

        unordered_set<string> locprm;

        vector<AssignStmt*> localStmts;
        for (pair<Decl*, Exp*> local : f->locals) {
            flir->locals[local.first->name] = local.first->type;
            locprm.insert(local.first->name);
            if (local.second) {
                localStmts.push_back(ConvertToAssign(local.first, local.second));
            }
        }

        for (Decl* prm : f->params) {
            locprm.insert(prm->name);
        }

        this->lblCounter = 1;
        this->varCounter = 1;
        this->numRet = 0;

        for (AssignStmt* s : localStmts) {
            StmtLow(locprm, s, flir);
        }
        for (Stmt* s : f->stmts) {
            StmtLow(locprm, s, flir);
        }

        shared_ptr<Label> EXIT = make_shared<Label>("EXIT", "error");
        Var* x;
        if (this->numRet > 1) {
            vector<shared_ptr<LirInstructions>> temp;
            
            if (this->translation.back().get()->get_type() == "LABEL") {
                Label* t = dynamic_cast<Label*>(this->translation.back().get());
                EXIT = make_shared<Label>("EXIT", t->name);
                this->translation.pop_back();
            }
            else {
                EXIT = make_shared<Label>("EXIT", lblIncrementer());
            }
            this->translation.push_back(EXIT);

            if (f->rettyp) {
                x = new Var(varIncrementer());
                flir->locals[x->id] = f->rettyp;

                this->translation.push_back(make_shared<Ret>(x));
            }
            else {
                this->translation.push_back(make_shared<Ret>(nullptr));
            }
        }

        int left = 0;
        string stopper = EXIT->name;
        while (left < this->translation.size()) {
            shared_ptr<LirInstructions> left_inst = this->translation[left];
            if (left_inst->get_type() != "LABEL") {
                left++;
                continue;
            }
            
            Label* label = dynamic_cast<Label*>(left_inst.get());
 
            BasicBlockLIR* bb = new BasicBlockLIR();
            bb->label = label->name;
            
            if (label->label == "WHILE_BODY") {
                int counter = stoi(label->name.substr(3, label->name.length() - 3));
                loop(label, left, bb, "lbl" + std::to_string(counter + 1), flir);
            }
            else if (label->label == "TT") {
                conditional(label, left, bb, stopper, flir);
            }
            else {
                translate(label, left, bb, stopper, flir);
            }
        }

        if (this->numRet > 1) {
            for (auto bb : flir->body) {
                if (bb.first != EXIT->name && bb.second->term->get_type() == "RET") {
                    if (f->rettyp) {
                        Ret* r = dynamic_cast<Ret*>(bb.second->term.get());
                        Operand* rop = r->op;
                        Operand* op;
                        if (rop->get_type() == "CONST") {
                            Const* cnst = dynamic_cast<Const*>(rop);
                            op = new Const(cnst->num);
                        }
                        if (rop->get_type() == "VAR") {
                            Var* vr = dynamic_cast<Var*>(rop);
                            op = new Var(vr->id);
                        }
                        bb.second->insts.push_back(make_shared<Copy>(x->id, op));
                        bb.second->term = make_shared<Jump>(EXIT);
                    }
                    else {
                        bb.second->term = make_shared<Jump>(EXIT);
                    }
                }
            }
        }
        
        this->functionsLIR[f->name] = flir;
    }
}

bool ProgramLIR::loop(Label* WHILE, int& left, BasicBlockLIR* bb, string stopper, FunctionLIR* flir) {
    int counter = stoi(WHILE->name.substr(3, WHILE->name.length() - 3));
    string next_stopper = "lbl" + std::to_string(counter + 1);

    translate(new Label("tempWHILE", WHILE->name), left, bb, stopper, flir);
    
    while (left < this->translation.size()) {
        shared_ptr<LirInstructions> left_inst = this->translation[left];

        if (left_inst->get_type() != "LABEL" ) {
            left++;
            continue;
        }
        
        Label* label = dynamic_cast<Label*>(left_inst.get());

        if (label->name == next_stopper) {
            break;
        }
        
        BasicBlockLIR* bb = new BasicBlockLIR();
        bb->label = label->name;

        if (label->label == "WHILE_BODY") {
            loop(label, left, bb, next_stopper, flir);
        }
        else if (label->label == "TT") {
            conditional(label, left, bb, next_stopper, flir);
        }
        else {
            translate(label, left, bb, next_stopper, flir);
        }
    }
    return false;
}

bool ProgramLIR::conditional(Label* TT, int& left, BasicBlockLIR* bb, string stopper, FunctionLIR* flir) {
    bool isTT = false;
    bool isFF = false;

    int counter = stoi(TT->name.substr(3, TT->name.length() - 3));
    string FF_stopper = "lbl" + std::to_string(counter + 1);
    string END_stopper = "lbl" + std::to_string(counter + 2);
    string next_stopper;

    if (translate(new Label("tempTT", TT->name), left, bb, "lbl" + std::to_string(counter + 1), flir)) {
        isTT = true;
    }

    while (left < this->translation.size()) {
        shared_ptr<LirInstructions> left_inst = this->translation[left];

        if (left_inst->get_type() != "LABEL" ) {
            left++;
            continue;
        }
        
        Label* label = dynamic_cast<Label*>(left_inst.get());

        if (label->name == FF_stopper) {
            break;
        }

        BasicBlockLIR* bb = new BasicBlockLIR();
        bb->label = label->name;

        int label_counter = stoi(label->name.substr(3, label->name.length() - 3));

        if (label->label == "WHILE_BODY") {
            next_stopper = "lbl" + std::to_string(label_counter + 1);
            if (loop(label, left, bb, next_stopper, flir)) {
                isTT = true;
            }
        }
        else if (label->label == "TT") {
            next_stopper = "lbl" + std::to_string(counter + 1);
            if (conditional(label, left, bb, next_stopper, flir)) {
                isTT = true;
            };
        }
        else {
            next_stopper = "lbl" + std::to_string(counter + 1);
            if (translate(label, left, bb, next_stopper, flir)) {
                isTT = true;
            }
        }
    }

    while (left < this->translation.size()) {
        shared_ptr<LirInstructions> left_inst = this->translation[left];
        if (left_inst->get_type() != "LABEL" ) {
            left++;
            continue;
        }
        
        Label* label = dynamic_cast<Label*>(left_inst.get());
        if (label->name == END_stopper) {
            break;
        }

        BasicBlockLIR* bb = new BasicBlockLIR();
        bb->label = label->name;

        int label_counter = stoi(label->name.substr(3, label->name.length() - 3));

        if (label->label == "WHILE_BODY") {
            next_stopper = "lbl" + std::to_string(label_counter + 1);
            if (loop(label, left, bb, next_stopper, flir)) {
                isFF = true;
            }
        }
        else if (label->label == "TT") {
            next_stopper = "lbl" + std::to_string(counter + 2);
            if (conditional(label, left, bb, next_stopper, flir)) {
                isFF = true;
            };
        }
        else {
            next_stopper = "lbl" + std::to_string(counter + 2);
            if (translate(label, left, bb, next_stopper, flir)) {
                isFF = true;
            }
        }
    }

    if (isTT && isFF) {
        while (left < this->translation.size()) {
             shared_ptr<LirInstructions> left_inst = this->translation[left];
            if (left_inst->get_type() != "LABEL" ) {
                left++;
                continue;
            }

            Label* label = dynamic_cast<Label*>(left_inst.get());

            if (label->name == stopper) {
                break;
            }
            left++;
        }
    }

    return isTT && isFF;
}

bool ProgramLIR::translate(Label* label, int& left, BasicBlockLIR* bb, string stopper, FunctionLIR* flir) {
    left++;
    while (left < this->translation.size()) {
        shared_ptr<LirInstructions> left_inst = this->translation[left++];
        string left_inst_type = left_inst->get_type();

        if (left_inst_type == "ALLOC") {
            Alloc* inst = dynamic_cast<Alloc*>(left_inst.get());
            bb->insts.push_back(shared_ptr<Alloc>(left_inst, inst));
        }
        else if (left_inst_type == "ARITH") {
            Arith* inst = dynamic_cast<Arith*>(left_inst.get());
            bb->insts.push_back(shared_ptr<Arith>(left_inst, inst));
        }
        else if (left_inst_type == "CALLEXT") {
            CallExt* inst = dynamic_cast<CallExt*>(left_inst.get());
            bb->insts.push_back(shared_ptr<CallExt>(left_inst, inst));
        }
        else if (left_inst_type == "CMP") {
            Cmp* inst = dynamic_cast<Cmp*>(left_inst.get());
            bb->insts.push_back(shared_ptr<Cmp>(left_inst, inst));
        }
        else if (left_inst_type == "COPY") {
            Copy* inst = dynamic_cast<Copy*>(left_inst.get());
            bb->insts.push_back(shared_ptr<Copy>(left_inst, inst));
        }
        else if (left_inst_type == "GEP") {
            Gep* inst = dynamic_cast<Gep*>(left_inst.get());
            bb->insts.push_back(shared_ptr<Gep>(left_inst, inst));
        }
        else if (left_inst_type == "GFP") {
            Gfp* inst = dynamic_cast<Gfp*>(left_inst.get());
            bb->insts.push_back(shared_ptr<Gfp>(left_inst, inst));
        }
        else if (left_inst_type == "LOAD") {
            Load* inst = dynamic_cast<Load*>(left_inst.get());
            bb->insts.push_back(shared_ptr<Load>(left_inst, inst));
        }
        else if (left_inst_type == "STORE") {
            Store* inst = dynamic_cast<Store*>(left_inst.get());
            bb->insts.push_back(shared_ptr<Store>(left_inst, inst));
        }
        else if (left_inst_type == "BRANCH") {
            Branch* terminal = dynamic_cast<Branch*>(left_inst.get());
            bb->term = shared_ptr<Branch>(left_inst, terminal);
            flir->body[label->name] = bb;
            return false;
        }
        else if (left_inst_type == "CALLDIRECT") {
            CallDirect* terminal = dynamic_cast<CallDirect*>(left_inst.get());
            bb->term = shared_ptr<CallDirect>(left_inst, terminal);
            flir->body[label->name] = bb;
            return false;
        }
        else if (left_inst_type == "CALLINDIRECT") {
            CallIndirect* terminal = dynamic_cast<CallIndirect*>(left_inst.get());
            bb->term = shared_ptr<CallIndirect>(left_inst, terminal);
            flir->body[label->name] = bb;
            return false;
        }
        else if (left_inst_type == "JUMP") {
            Jump* terminal = dynamic_cast<Jump*>(left_inst.get());
            bb->term = shared_ptr<Jump>(left_inst, terminal);
            flir->body[label->name] = bb;
            if (terminal->next_bb->label == "BREAK/CONT") {
                return true;
            }
            return false;
        }
        else if (left_inst_type == "RET") {
            Ret* terminal = dynamic_cast<Ret*>(left_inst.get());
            bb->term = shared_ptr<Ret>(left_inst, terminal);
            flir->body[label->name] = bb;
            return true;
        }
        else {
            cout << "error" << endl;
        }
    }
    return false;
}

void ProgramLIR::StmtLow(unordered_set<string>& locprm, Stmt* s, FunctionLIR* flir) {
    if (s->get_type() == "IF") {
        IfStmt* i = dynamic_cast<IfStmt*>(s);
        IfStmtLow(locprm, i, flir);
    }
    else if (s->get_type() == "WHILE") {
        WhileStmt* w = dynamic_cast<WhileStmt*>(s);
        WhileStmtLow(locprm, w, flir);
    }
    else if (s->get_type() == "ASSIGN") {
        AssignStmt* a = dynamic_cast<AssignStmt*>(s);
        AssignStmtLow(locprm, a, flir);
    }
    else if (s->get_type() == "SCALL") {
        CallStmt* call = dynamic_cast<CallStmt*>(s);
        CallStmtLow(locprm, call, flir);
    }
    else if (s->get_type() == "CONTINUE") {
        ContStmtLow();
    }
    else if (s->get_type() == "BREAK") {
        BreakStmtLow();
    }
    else if (s->get_type() == "RETURN") {
        ReturnStmt* r = dynamic_cast<ReturnStmt*>(s);
        RetStmtLow(locprm, r, flir);
    }
    else {
        return;
    }
}

void ProgramLIR::IfStmtLow(unordered_set<string>& locprm, IfStmt* i, FunctionLIR* flir) {
    shared_ptr<Label> TT = make_shared<Label>("TT", lblIncrementer());
    shared_ptr<Label> FF = make_shared<Label>("FF", lblIncrementer());
    shared_ptr<Label> IF_END = make_shared<Label>("IF_END", lblIncrementer());

    this->translation.push_back(make_shared<Branch>(ExpLow(locprm, i->guard, flir), TT, FF));
    this->translation.push_back(TT);
    for (Stmt* tt : i->tt) {
        StmtLow(locprm, tt, flir);
    }
    this->translation.push_back(make_shared<Jump>(IF_END));
    this->translation.push_back(FF);
    for (Stmt* ff : i->ff) {
        StmtLow(locprm, ff, flir);
    }
    this->translation.push_back(make_shared<Jump>(IF_END));
    this->translation.push_back(IF_END);
}

void ProgramLIR::WhileStmtLow(unordered_set<string>& locprm, WhileStmt* w, FunctionLIR* flir) {
    string hdr = lblIncrementer();
    this->while_hdrs.push(hdr);
    shared_ptr<Label> WHILE_HDR = make_shared<Label>("WHILE_HDR", hdr);
    shared_ptr<Label> WHILE_BODY = make_shared<Label>("WHILE_BODY", lblIncrementer());
    string end = lblIncrementer();
    this->while_ends.push(end);
    shared_ptr<Label> WHILE_END = make_shared<Label>("WHILE_END", end);

    this->translation.push_back(make_shared<Jump>(WHILE_HDR));
    this->translation.push_back(WHILE_HDR);
    this->translation.push_back(make_shared<Branch>(ExpLow(locprm, w->guard, flir), WHILE_BODY, WHILE_END));
    this->translation.push_back(WHILE_BODY);
    for (Stmt* body : w->body) {
        StmtLow(locprm, body, flir);
    }
    this->translation.push_back(make_shared<Jump>(WHILE_HDR));
    this->translation.push_back(WHILE_END);

    this->while_hdrs.pop();
    this->while_ends.pop();
}

void ProgramLIR::AssignStmtLow(unordered_set<string>& locprm, AssignStmt* a, FunctionLIR* flir) {
    Lval* lhs = a->lhs;
    Rhs* rhs = a->rhs;

    if (rhs->get_type() == "EXP") {
        RhsExp* rhs_exp = dynamic_cast<RhsExp*>(rhs);
        Exp* e = rhs_exp->exp;
        if (lhs->get_type() == "ID") {
            IdLval* id = dynamic_cast<IdLval*>(lhs);
            this->translation.push_back(make_shared<Copy>(id->name, ExpLow(locprm, e, flir)));
        }
        else {
            Var* x = LvalLow(locprm, lhs, flir);
            Operand* y = ExpLow(locprm, e, flir);
            this->translation.push_back(make_shared<Store>(x->id, y));
        }
    }
    if (rhs->get_type() == "NEW") {
        New* rhs_new = dynamic_cast<New*>(rhs);
        shared_ptr<Type> typ = rhs_new->type;
        Exp* e = rhs_new->amount;
        if (lhs->get_type() == "ID") {
            IdLval* id = dynamic_cast<IdLval*>(lhs);
            this->translation.push_back(make_shared<Alloc>(id->name, ExpLow(locprm, e, flir)));
        }
        else {
            Var* w = new Var(varIncrementer());
            flir->locals[w->id] = make_shared<PtrType>(typ);
            Var* x = LvalLow(locprm, lhs, flir);

            this->translation.push_back(make_shared<Alloc>(w->id, ExpLow(locprm, e, flir)));
            this->translation.push_back(make_shared<Store>(x->id, w));
        }
    }
}

void ProgramLIR::CallStmtLow(unordered_set<string>& locprm, CallStmt* call, FunctionLIR* flir) {
    vector<Operand*> aops;
    for (Exp* a : call->args) {
        aops.push_back(ExpLow(locprm, a, flir));
    }

    IdLval* id = dynamic_cast<IdLval*>(call->callee);
    
    bool direct = id && (locprm.find(id->name) == locprm.end());
    if (direct && this->ExternsLIR.find(id->name) != this->ExternsLIR.end()) {
        this->translation.push_back(make_shared<CallExt>(nullptr, id->name, aops));
    }
    else {
        shared_ptr<Label> NEXT = make_shared<Label>("NEXT", lblIncrementer());
        if (direct && this->GlobalsLIR.find(id->name) != this->GlobalsLIR.end() && this->GlobalsLIR[id->name]->get_type().substr(0, 2) == "&(") {
            this->translation.push_back(make_shared<CallDirect>(nullptr, id->name, aops, NEXT->name));
        }
        else {
            this->translation.push_back(make_shared<CallIndirect>(nullptr, LvalExpLow(locprm, call->callee, flir)->id, aops, NEXT->name));
        }
        this->translation.push_back(NEXT);
    }
}

void ProgramLIR::ContStmtLow() {
    for (int i = this->translation.size() - 1; i >= 0; i--) {
        Label* prev = dynamic_cast<Label*>(this->translation[i].get());
        if (prev && prev->name == this->while_hdrs.top()) {
            shared_ptr<Label> temp = make_shared<Label>("BREAK/CONT", prev->name);
            this->translation.push_back(make_shared<Jump>(temp));
            break;
        }
    }
}

void ProgramLIR::BreakStmtLow() {
    for (int i = this->translation.size() - 1; i >= 0; i--) {
        Branch* prev = dynamic_cast<Branch*>(this->translation[i].get());
        if (prev && prev->ff->name == this->while_ends.top()) {
            shared_ptr<Label> temp = make_shared<Label>("BREAK/CONT", prev->ff->name);
            this->translation.push_back(make_shared<Jump>(temp));
            break;
        }
    }
}

void ProgramLIR::RetStmtLow(unordered_set<string>& locprm, ReturnStmt* r, FunctionLIR* flir) {
    if (!r->exp) {
        this->translation.push_back(make_shared<Ret>(nullptr));
    }
    else {
        this->translation.push_back(make_shared<Ret>(ExpLow(locprm, r->exp, flir)));
    }
    this->numRet++;
}


Operand* ProgramLIR::ExpLow(unordered_set<string>& locprm, Exp* e, FunctionLIR* flir) {
    if (e->get_type() == "NUM") {
        NumExp* num = dynamic_cast<NumExp*>(e);
        return NumExpLow(num);
    }
    else if (e->get_type() == "ID") {
        IdExp* id = dynamic_cast<IdExp*>(e);
        return IdExpLow(id);
    }
    else if (e->get_type() == "NIL") {
        return NilExpLow();
    }
    else if (e->get_type() == "NEG") {
        UnOpExp* neg = dynamic_cast<UnOpExp*>(e);
        return NegExpLow(locprm, neg, flir);
    }
    else if (e->get_type() == "DEREF") {
        UnOpExp* deref = dynamic_cast<UnOpExp*>(e);
        return DerefExpLow(locprm, deref, flir);
    }
    else if (e->get_type() == "BINOP-REST") {
        BinOpExp* binoprest = dynamic_cast<BinOpExp*>(e);
        return BinOpRestExpLow(locprm, binoprest, flir);
    }
    else if (e->get_type() == "BINOP-EQ") {
        BinOpExp* binopeq = dynamic_cast<BinOpExp*>(e);
        return BinOpEqExpLow(locprm, binopeq, flir);
    }
    else if (e->get_type() == "ARRAY") {
        ArrayAccessExp* arrayaccess = dynamic_cast<ArrayAccessExp*>(e);
        return ArrayAccExpLow(locprm, arrayaccess, flir);
    }
    else if (e->get_type() == "FIELD") {
        FieldAccessExp* fieldaccess = dynamic_cast<FieldAccessExp*>(e);
        return FieldAccExpLow(locprm, fieldaccess, flir);
    }
    else if (e->get_type() == "ECALL") {
        CallExp* call = dynamic_cast<CallExp*>(e);
        return CallExpLow(locprm, call, flir);
    }
    else {
        return nullptr;
    }
}

Operand* ProgramLIR::NumExpLow(NumExp* num) {
    int32_t n = num->n;
    return new Const(n);
}

Operand* ProgramLIR::IdExpLow(IdExp* id) {
    string name = id->name;
    return new Var(name);
}

Operand* ProgramLIR::NilExpLow() {
    return new Const(0);
}

Operand* ProgramLIR::NegExpLow(unordered_set<string>& locprm, UnOpExp* neg, FunctionLIR* flir) {
    Var* lhs = new Var(varIncrementer());
    flir->locals[lhs->id] = make_shared<IntType>();
    this->translation.push_back(make_shared<Arith>(lhs->id, new SubLIR(), new Const(0), ExpLow(locprm, neg->operand, flir)));
    return lhs;
}

Operand* ProgramLIR::DerefExpLow(unordered_set<string>& locprm, UnOpExp* deref, FunctionLIR* flir) {
    Var* src = dynamic_cast<Var*>(ExpLow(locprm, deref->operand, flir));
    if (!src) {
        cout << "Error at DerefExpLow" << endl;
    }

    shared_ptr<Type> shared_src_type = findType(src->id, flir);
    PtrType* src_type = dynamic_cast<PtrType*>(shared_src_type.get());
    if (!src_type) {
        cout << "Error at DerefExpLow" << endl;
    }

    Var* lhs = new Var(varIncrementer());
    flir->locals[lhs->id] = src_type->ref;

    this->translation.push_back(make_shared<Load>(lhs->id, src->id));
    return lhs;
}

Operand* ProgramLIR::BinOpRestExpLow(unordered_set<string>& locprm, BinOpExp* binoprest, FunctionLIR* flir) {
    ArithmeticOp* op;
    if (binoprest->op->to_string() == "Add") {
        op = new AddLIR();
    }
    else if (binoprest->op->to_string() == "Sub") {
        op = new SubLIR();
    }
    else if (binoprest->op->to_string() == "Mul") {
        op = new MultLIR();
    }
    else if (binoprest->op->to_string() == "Div") {
        op = new DivLIR();
    }
    Operand* op1 = ExpLow(locprm, binoprest->left, flir);
    Operand* op2 = ExpLow(locprm, binoprest->right, flir);
    Var* lhs = new Var(varIncrementer());
    flir->locals[lhs->id] = make_shared<IntType>();
    this->translation.push_back(make_shared<Arith>(lhs->id, op, op1, op2));
    return lhs;
}

Operand* ProgramLIR::BinOpEqExpLow(unordered_set<string>& locprm, BinOpExp* binopeq, FunctionLIR* flir) {
    ComparisonOp* op;
    if (binopeq->op->to_string() == "Equal") {
        op = new EqualLIR();
    }
    else if (binopeq->op->to_string() == "NotEq") {
        op = new NotEqLIR();
    }
    else if (binopeq->op->to_string() == "Lt") {
        op = new LtLIR();
    }
    else if (binopeq->op->to_string() == "Lte") {
        op = new LteLIR();
    }
    else if (binopeq->op->to_string() == "Gt") {
        op = new GtLIR();
    }
    else if (binopeq->op->to_string() == "Gte") {
        op = new GteLIR();
    }
    Operand* op1 = ExpLow(locprm, binopeq->left, flir);
    Operand* op2 = ExpLow(locprm, binopeq->right, flir);
    Var* lhs = new Var(varIncrementer());
    flir->locals[lhs->id] = make_shared<IntType>();
    this->translation.push_back(make_shared<Cmp>(lhs->id, op, op1, op2));
    return lhs;
}

Operand* ProgramLIR::ArrayAccExpLow(unordered_set<string>& locprm, ArrayAccessExp* arrayaccess, FunctionLIR* flir) {
    Var* src = dynamic_cast<Var*>(ExpLow(locprm, arrayaccess->ptr, flir));
    if (!src) {
        cout << "Error at ArrayAccExpLow" << endl;
    }

    Operand* idx = ExpLow(locprm, arrayaccess->index, flir);
    
    shared_ptr<Type> shared_src_type = findType(src->id, flir);
    PtrType* src_type = dynamic_cast<PtrType*>(shared_src_type.get());
    if (!src_type) {
        cout << "Error at ArrayAccExpLow" << endl;
    }

    Var* elem = new Var(varIncrementer());
    flir->locals[elem->id] = shared_src_type;
    Var* lhs = new Var(varIncrementer());
    flir->locals[lhs->id] = src_type->ref;
    this->translation.push_back(make_shared<Gep>(elem->id, src->id, idx));
    this->translation.push_back(make_shared<Load>(lhs->id, elem->id));
    return lhs;
}

Operand* ProgramLIR::FieldAccExpLow(unordered_set<string>& locprm, FieldAccessExp* fieldaccess, FunctionLIR* flir) {
    Var* src = dynamic_cast<Var*>(ExpLow(locprm, fieldaccess->ptr, flir));
    if (!src) {
        cout << "Error at FieldAccExpLow" << endl;
    }

    shared_ptr<Type> shared_src_type = findType(src->id, flir);
    PtrType* src_type = dynamic_cast<PtrType*>(shared_src_type.get());
    if (!src_type) {
        cout << "Error at LvalFieldAccessLow" << endl;
    }

    StructType* struct_type = dynamic_cast<StructType*>(src_type->ref.get());
    if (!struct_type) {
        cout << "Error at FieldAccExpLow" << endl;
    }

    shared_ptr<Type> field_type = this->StructsLIR[struct_type->name][fieldaccess->field];
    if (!src_type) {
        cout << "Error at FieldAccExpLow" << endl;
    }

    Var* fldp = new Var(varIncrementer());
    flir->locals[fldp->id] = make_shared<PtrType>(field_type);
    Var* lhs = new Var(varIncrementer());
    flir->locals[lhs->id] = field_type;
    this->translation.push_back(make_shared<Gfp>(fldp->id, src->id, fieldaccess->field));
    this->translation.push_back(make_shared<Load>(lhs->id, fldp->id));
    return lhs;
}

Operand* ProgramLIR::CallExpLow(unordered_set<string>& locprm, CallExp* call, FunctionLIR* flir) {
    vector<Operand*> aops;
    for (Exp* a : call->args) {
        aops.push_back(ExpLow(locprm, a, flir));
    }

    IdExp* id = dynamic_cast<IdExp*>(call->callee);

    bool direct = id && (locprm.find(id->name) == locprm.end());
    Var* fun = dynamic_cast<Var*>(ExpLow(locprm, call->callee, flir));

    PtrType* fun_type;
    FnType* deref_fun_type;

    Var* lhs = new Var(varIncrementer());
    if (direct && this->ExternsLIR.find(id->name) != this->ExternsLIR.end()) {
        deref_fun_type = dynamic_cast<FnType*>(this->ExternsLIR[id->name].get());
        this->translation.push_back(make_shared<CallExt>(new string(lhs->id), id->name, aops));
    }
    else {
        shared_ptr<Label> NEXT = make_shared<Label>("NEXT", lblIncrementer());    
        if (direct && this->GlobalsLIR.find(id->name) != this->GlobalsLIR.end() && this->GlobalsLIR[id->name]->get_type().substr(0, 2) == "&(") {
            fun_type = dynamic_cast<PtrType*>(this->GlobalsLIR[fun->id].get());
            deref_fun_type = dynamic_cast<FnType*>(fun_type->ref.get());
            this->translation.push_back(make_shared<CallDirect>(new string(lhs->id), id->name, aops, NEXT->name));
        }
        else {
            fun_type = dynamic_cast<PtrType*>(flir->locals[fun->id].get());
            deref_fun_type = dynamic_cast<FnType*>(fun_type->ref.get());
            this->translation.push_back(make_shared<CallIndirect>(new string(lhs->id), fun->id, aops, NEXT->name));
        }
        this->translation.push_back(NEXT);
    }

    if (!deref_fun_type) {
        cout << "Error at CallExpLow" << endl;
    }
    flir->locals[lhs->id] = deref_fun_type->ret;
    return lhs;
}


Var* ProgramLIR::LvalLow(unordered_set<string>& locprm, Lval* l, FunctionLIR* flir) {
    if (l->get_type() == "DEREF") {
        DerefLval* deref = dynamic_cast<DerefLval*>(l);
        return LvalDerefLow(locprm, deref, flir);
    }
    else if (l->get_type() == "ARRAY") {
        ArrayAccessLval* arrayaccess = dynamic_cast<ArrayAccessLval*>(l);
        return LvalArrayAccessLow(locprm, arrayaccess, flir);
    }
    else if (l->get_type() == "FIELD") {
        FieldAccessLval* fieldaccess = dynamic_cast<FieldAccessLval*>(l);
        return LvalFieldAccessLow(locprm, fieldaccess, flir);
    }
    else {
        return nullptr;
    }
}

Var* ProgramLIR::LvalDerefLow(unordered_set<string>& locprm, DerefLval* deref, FunctionLIR* flir) {
    return LvalExpLow(locprm, deref->lval, flir);
}

Var* ProgramLIR::LvalArrayAccessLow(unordered_set<string>& locprm, ArrayAccessLval* arrayaccess, FunctionLIR* flir) {
    Var* src = LvalExpLow(locprm, arrayaccess->ptr, flir);
    Operand* idx = ExpLow(locprm, arrayaccess->index, flir);
    Var* lhs = new Var(varIncrementer());
    flir->locals[lhs->id] = findType(src->id, flir);
    this->translation.push_back(make_shared<Gep>(lhs->id, src->id, idx));
    return lhs;
}

Var* ProgramLIR::LvalFieldAccessLow(unordered_set<string>& locprm, FieldAccessLval* fieldaccess, FunctionLIR* flir) {
    Var* src = LvalExpLow(locprm, fieldaccess->ptr, flir);
    Var* lhs = new Var(varIncrementer());
   
    shared_ptr<Type> shared_src_type = findType(src->id, flir);
    PtrType* src_type = dynamic_cast<PtrType*>(shared_src_type.get());
    if (!src_type) {
        cout << "Error at LvalFieldAccessLow" << endl;
    }
    StructType* struct_type = dynamic_cast<StructType*>(src_type->ref.get());
    if (!struct_type) {
        cout << "Error at LvalFieldAccessLow" << endl;
    }

    flir->locals[lhs->id] = make_shared<PtrType>(this->StructsLIR[struct_type->name][fieldaccess->field]);
    this->translation.push_back(make_shared<Gfp>(lhs->id, src->id, fieldaccess->field));
    return lhs;
}

Var* ProgramLIR::LvalExpLow(unordered_set<string>& locprm, Lval* lv, FunctionLIR* flir) {
    IdLval* id = dynamic_cast<IdLval*>(lv);
    if (id) {
        return new Var(id->name);
    }
    else {
        Var* src = LvalLow(locprm, lv, flir);

        PtrType* src_type = dynamic_cast<PtrType*>(findType(src->id, flir).get());
        if (!src_type) {
            cout << "Error at LvalExpLow" << endl;
        }

        Var* lhs = new Var(varIncrementer());
        flir->locals[lhs->id] = src_type->ref;
        this->translation.push_back(make_shared<Load>(lhs->id, src->id));
        return lhs;
    }
}


ProgramLIR::~ProgramLIR() {
    for (auto mapping : this->functionsLIR) {
        delete mapping.second;
    }
}

ProgramLIR::ProgramLIR() {}

string ProgramLIR::to_string() const {
    string val = "";
    for (auto struc : this->StructsLIR) {
        val += "Struct " + struc.first;
        for (auto entry : struc.second) {
            val += "\n  " + entry.first + " : " + entry.second->to_string();
        }
        val += "\n\n";
    }

    val += "Externs";
    for (auto exter : this->ExternsLIR) {
        val += "\n  " + exter.first + " : " + exter.second->print_type();
    }

    val += "\n\nGlobals";
    for (auto glob : this->GlobalsLIR) {
        val += "\n  " + glob.first + " : " + glob.second->to_string();
    }

    for (auto func : this->functionsLIR) {
        val += "\n\nFunction " + func.first + func.second->to_string();
    }

    return val;
}
