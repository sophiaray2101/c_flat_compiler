#include "../../include/codegen/FunctionCG.h"
#include<iostream>

#include <iostream>

BlockCG::BlockCG() {}
BlockCG::~BlockCG() {
    for (auto inst : this->insts) {
        delete inst;
    }
}
string BlockCG::to_string() const {
    string ret = this->name + ":";
    for (auto inst : this->insts) {
        ret += "\n  " + inst->to_string();
    }
    ret += "\n\n";
    return ret;
}
void FunctionCG::convertCopyX(vector<XInsts*>& insts, Copy* copyLIR){
    if (copyLIR->op->get_type() == "CONST") {
        Const* op = dynamic_cast<Const*>(copyLIR->op);
        insts.push_back(new movq(make_shared<DollSign>(op->num),this->locals[copyLIR->lhs]));
    }
    else {
        Var* op = dynamic_cast<Var*>(copyLIR->op);
        shared_ptr<Reg> r = make_shared<Reg>("r8");
        insts.push_back(new movq(this->locals[op->id], r));
        insts.push_back(new movq(r, this->locals[copyLIR->lhs]));
    }
}

void FunctionCG::convertCmpX(vector<XInsts*>& insts, Cmp* cmpLIR){
    shared_ptr<Reg> r = make_shared<Reg>("r8");
    string cmpType = cmpLIR->aop->to_string().substr(0,1);
    if(cmpLIR->aop->to_string() == "gte" || cmpLIR->aop->to_string() == "lte" || cmpLIR->aop->to_string() == "neq") {
        cmpType += "e";
    }
    if(cmpLIR->left->get_type() == "CONST" && cmpLIR->right->get_type() == "CONST"){
        Const* left = dynamic_cast<Const*>(cmpLIR->left);
        Const* right = dynamic_cast<Const*>(cmpLIR->right);
        insts.push_back(new movq(make_shared<DollSign>(left->num),r));
        insts.push_back(new cmpq(make_shared<DollSign>(right->num), r));
        insts.push_back(new movq(make_shared<DollSign>(0),r));
        insts.push_back(new setX(make_shared<Reg>("r8b"),cmpType));
        insts.push_back(new movq(r, this->locals[cmpLIR->lhs]));
    }
    else if(cmpLIR->left->get_type() == "VAR" && cmpLIR->right->get_type() == "VAR"){
        Var* left = dynamic_cast<Var*>(cmpLIR->left);
        Var* right = dynamic_cast<Var*>(cmpLIR->right);
        insts.push_back(new movq(this->locals[left->id],r));
        insts.push_back(new cmpq(this->locals[right->id], r));
        insts.push_back(new movq(make_shared<DollSign>(0),r));
        insts.push_back(new setX(make_shared<Reg>("r8b"),cmpType));
        insts.push_back(new movq(r, this->locals[cmpLIR->lhs]));
    }
    else{
        if(cmpLIR->left->get_type() == "VAR"){
            Var* left = dynamic_cast<Var*>(cmpLIR->left);
            Const* right = dynamic_cast<Const*>(cmpLIR->right);
            insts.push_back(new cmpq(make_shared<DollSign>(right->num), this->locals[left->id]));
        }
        else{
            Const* left = dynamic_cast<Const*>(cmpLIR->left);
            Var* right = dynamic_cast<Var*>(cmpLIR->right);
            insts.push_back(new movq(make_shared<DollSign>(left->num),r));
            insts.push_back(new cmpq(this->locals[right->id], r));
        }
        insts.push_back(new movq(make_shared<DollSign>(0),r));
        insts.push_back(new setX(make_shared<Reg>("r8b"),cmpType));
        insts.push_back(new movq(r,this->locals[cmpLIR->lhs]));
    }
}

void FunctionCG::convertRetX(vector<XInsts*>& insts, Ret* ret) {
    if (ret->op) {
        shared_ptr<Reg> RAX = make_shared<Reg>("rax");
        if (ret->op->get_type() == "CONST") {
            Const* cnst = dynamic_cast<Const*>(ret->op);
            insts.push_back(new movq(make_shared<DollSign>(cnst->num), RAX));
        }
        if (ret->op->get_type() == "VAR") {
            Var* vr = dynamic_cast<Var*>(ret->op);
            insts.push_back(new movq(this->locals[vr->id], RAX));
        }
    }
    insts.push_back(new jmp(this->epilogue->name));
}

void FunctionCG::convertBranchX(vector<XInsts*>& insts, Branch* branch) {
    Operand* guard = branch->guard; 
    if (guard->get_type() == "CONST") {
        Const* cnst = dynamic_cast<Const*>(guard);
        insts.push_back(new movq(make_shared<DollSign>(cnst->num), make_shared<Reg>("r8")));
        insts.push_back(new cmpq(make_shared<DollSign>(0), make_shared<Reg>("r8")));
    }
    if (guard->get_type() == "VAR") {
        Var* vr = dynamic_cast<Var*>(guard);
        insts.push_back(new cmpq(make_shared<DollSign>(0), this->locals[vr->id]));
    }
    insts.push_back(new jX("ne", this->name + "_" + branch->tt->name));
    insts.push_back(new jmp(this->name + "_" + branch->ff->name));
}

void FunctionCG::convertJumpX(vector<XInsts*>& insts, Jump* jump) {
    insts.push_back(new jmp(this->name + "_" + jump->next_bb->name));
}

void FunctionCG::convertCallExtX(vector<XInsts*>& insts, CallExt* callext) {
    vector<string> registers = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    vector<Operand*> args = callext->args;
    for (int i = 0; i < args.size() && i < 6; i++) {
        if (args[i]->get_type() == "CONST") {
            Const* cnst = dynamic_cast<Const*>(args[i]);
            insts.push_back(new movq(make_shared<DollSign>(cnst->num), make_shared<Reg>(registers[i])));
        }
        if (args[i]->get_type() == "VAR") {
            Var* vr = dynamic_cast<Var*>(args[i]);
            insts.push_back(new movq(this->locals[vr->id], make_shared<Reg>(registers[i])));
        }
    }
    for (int i = args.size() - 1; i >= 6; i--) {
        if (args[i]->get_type() == "CONST") {
            Const* cnst = dynamic_cast<Const*>(args[i]);
            insts.push_back(new pushq(make_shared<DollSign>(cnst->num)));
        }
        if (args[i]->get_type() == "VAR") {
            Var* vr = dynamic_cast<Var*>(args[i]);
            insts.push_back(new pushq(this->locals[vr->id]));
        }
    }

    int alignment = args.size() - 6;
    if (args.size() > 6 && ((args.size() - 6) % 2 == 1)) {
        alignment++;
        insts.push_back(new subq(make_shared<DollSign>(8), make_shared<Reg>("rsp")));
    }

    insts.push_back(new call(callext->callee));
    if (callext->lhs) {
        insts.push_back(new movq(make_shared<Reg>("rax"), this->locals[*callext->lhs]));
    }
    if (args.size() > 6) {
        insts.push_back(new addq(make_shared<DollSign>(alignment * 8), make_shared<Reg>("rsp")));
    }
}

void FunctionCG::convertArithX(vector<XInsts*>& insts, Arith* arith) {
    shared_ptr<Reg> first = (arith->aop->to_string() == "div") ? make_shared<Reg>("rax") : make_shared<Reg>("r8");

    if (arith->left->get_type() == "CONST") {
        Const* cnst = dynamic_cast<Const*>(arith->left);
        insts.push_back(new movq(make_shared<DollSign>(cnst->num), first));
    }
    if (arith->left->get_type() == "VAR") {
        Var* vr = dynamic_cast<Var*>(arith->left);
        insts.push_back(new movq(this->locals[vr->id], first));
    }


    if (arith->aop->to_string() == "add") {
        if (arith->right->get_type() == "CONST") {
            Const* cnst = dynamic_cast<Const*>(arith->right);
            insts.push_back(new addq(make_shared<DollSign>(cnst->num), first));
        }
        if (arith->right->get_type() == "VAR") {
            Var* vr = dynamic_cast<Var*>(arith->right);
            insts.push_back(new addq(this->locals[vr->id], first));
        }
    }
    else if (arith->aop->to_string() == "sub") {
        if (arith->right->get_type() == "CONST") {
            Const* cnst = dynamic_cast<Const*>(arith->right);
            insts.push_back(new subq(make_shared<DollSign>(cnst->num), first));
        }
        if (arith->right->get_type() == "VAR") {
            Var* vr = dynamic_cast<Var*>(arith->right);
            insts.push_back(new subq(this->locals[vr->id], first));
        }
    }
    else if (arith->aop->to_string() == "mul") {
        if (arith->right->get_type() == "CONST") {
            Const* cnst = dynamic_cast<Const*>(arith->right);
            insts.push_back(new imulq(make_shared<DollSign>(cnst->num), first));
        }
        if (arith->right->get_type() == "VAR") {
            Var* vr = dynamic_cast<Var*>(arith->right);
            insts.push_back(new imulq(this->locals[vr->id], first));
        }
    }
    else if (arith->aop->to_string() == "div") {
        insts.push_back(new cqo());

        if (arith->right->get_type() == "CONST") {
            Const* cnst = dynamic_cast<Const*>(arith->right);
            insts.push_back(new movq(make_shared<DollSign>(cnst->num), make_shared<Reg>("r8")));
            insts.push_back(new idivq(make_shared<Reg>("r8")));
        }
        if (arith->right->get_type() == "VAR") {
            Var* vr = dynamic_cast<Var*>(arith->right);
            insts.push_back(new idivq(this->locals[vr->id]));
        }
    }

    insts.push_back(new movq(first, this->locals[arith->lhs]));
}

void FunctionCG::convertCallDirX(vector<XInsts*>& insts, CallDirect* calldir){
    int paramNum = calldir->args.size();
    for(auto it = calldir->args.rbegin(); it != calldir->args.rend(); ++it){
        Operand* arg = *it;
        if(arg->get_type() == "CONST"){
            Const* param = dynamic_cast<Const*>(arg);
            insts.push_back(new pushq(make_shared<DollSign>(param->num)));
        }
        else{
            Var* param = dynamic_cast<Var*>(arg);
            insts.push_back(new pushq(this->locals[param->id]));
        }
    }
    insts.push_back(new call(calldir->callee));
    if(calldir->lhs != nullptr){
        insts.push_back(new movq(make_shared<Reg>("rax"), this->locals[*calldir->lhs]));
    }
    insts.push_back(new addq(make_shared<DollSign>(8*paramNum),make_shared<Reg>("rsp")));
    convertJumpX(insts,new Jump(make_shared<Label>("TEMP", calldir->next_bb)));
}

void FunctionCG::convertCallIndirX(vector<XInsts*>& insts, CallIndirect* callindir){
    int paramNum = callindir->args.size();
    insts.push_back(new subq(make_shared<DollSign>(8), make_shared<Reg>("rsp")));
    for(auto it = callindir->args.rbegin(); it != callindir->args.rend(); ++it){
        Operand* arg = *it;
        if(arg->get_type() == "CONST"){
            Const* param = dynamic_cast<Const*>(arg);
            insts.push_back(new pushq(make_shared<DollSign>(param->num)));
        }
        else{
            Var* param = dynamic_cast<Var*>(arg);
            insts.push_back(new pushq(this->locals[param->id]));
        }
    }
    string callee = "*"+this->locals[callindir->callee]->to_string();
    insts.push_back(new call(callee));
    if(callindir->lhs != nullptr){
        insts.push_back(new movq(make_shared<Reg>("rax"), this->locals[*callindir->lhs]));
    }
    insts.push_back(new addq(make_shared<DollSign>((8*paramNum) + 8),make_shared<Reg>("rsp")));
    convertJumpX(insts,new Jump(make_shared<Label>("TEMP", callindir->next_bb)));
}

void FunctionCG::convertAllocX(vector<XInsts*>& insts, Alloc* alloc, FunctionLIR* flir, ProgramLIR& programLIR) {
    shared_ptr<XOps> arr_size;
    shared_ptr<XOps> multiplier;
    if (alloc->num->get_type() == "CONST") {
        Const* cnst = dynamic_cast<Const*>(alloc->num);

        arr_size = make_shared<DollSign>(cnst->num);
        multiplier = make_shared<Reg>("r8");

        insts.push_back(new movq(arr_size, make_shared<Reg>("r8")));
        insts.push_back(new cmpq(make_shared<DollSign>(0), make_shared<Reg>("r8")));
    }
    if (alloc->num->get_type() == "VAR") {
        Var* vr = dynamic_cast<Var*>(alloc->num);

        arr_size = this->locals[vr->id];
        multiplier = this->locals[vr->id];

        insts.push_back(new cmpq(make_shared<DollSign>(0), arr_size));
    }

    insts.push_back(new jX("le", ".invalid_alloc_length"));

    shared_ptr<Type> lhs_type = programLIR.findType(alloc->lhs, flir);
    PtrType* lhs_ptr = dynamic_cast<PtrType*>(lhs_type.get());
    if (!lhs_ptr) {
        cout << "error at convertAllocX" << endl;
    }

    int entry_size;
    string structID = lhs_ptr->ref->print_type();
    if (programLIR.StructsLIR.find(structID) != programLIR.StructsLIR.end()) {
        entry_size = programLIR.StructsLIR[structID].size();
    }
    else {
        entry_size = 1;
    }

    insts.push_back(new movq(make_shared<DollSign>(entry_size), make_shared<Reg>("rdi")));
    insts.push_back(new imulq(multiplier, make_shared<Reg>("rdi")));
    insts.push_back(new incq(make_shared<Reg>("rdi")));
    insts.push_back(new call("_cflat_alloc"));
    insts.push_back(new movq(arr_size, make_shared<Reg>("r8")));
    insts.push_back(new movq(make_shared<Reg>("r8"), make_shared<OffReg>("0", make_shared<Reg>("rax"))));
    insts.push_back(new addq(make_shared<DollSign>(8), make_shared<Reg>("rax")));
    insts.push_back(new movq(make_shared<Reg>("rax"), this->locals[alloc->lhs]));
}

void FunctionCG::convertGepX(vector<XInsts*>& insts, Gep* gep, FunctionLIR* flir, ProgramLIR& programLIR) {
    if (gep->idx->get_type() == "CONST"){
        Const* cnst = dynamic_cast<Const*>(gep->idx);
        insts.push_back(new movq(make_shared<DollSign>(cnst->num), make_shared<Reg>("r8")));
    }
    if (gep->idx->get_type() == "VAR") {
        Var* vr = dynamic_cast<Var*>(gep->idx);
        insts.push_back(new movq(this->locals[vr->id],  make_shared<Reg>("r8")));
    }

    insts.push_back(new cmpq(make_shared<DollSign>(0), make_shared<Reg>("r8")));
    insts.push_back(new jX("l", ".out_of_bounds"));
    insts.push_back(new movq(this->locals[gep->src], make_shared<Reg>("r9")));
    insts.push_back(new movq(make_shared<OffReg>("-8", make_shared<Reg>("r9")), make_shared<Reg>("r10")));
    insts.push_back(new cmpq(make_shared<Reg>("r10"), make_shared<Reg>("r8")));
    insts.push_back(new jX("ge", ".out_of_bounds"));

    shared_ptr<Type> src_type = programLIR.findType(gep->src, flir);
    PtrType* src_ptr = dynamic_cast<PtrType*>(src_type.get());
    if (!src_ptr) {
        cout << "error at convertGepX" << endl;
    }

    int entry_size;
    string structID = src_ptr->ref->print_type();
    if (programLIR.StructsLIR.find(structID) != programLIR.StructsLIR.end()) {
        entry_size = programLIR.StructsLIR[structID].size();
    }
    else {
        entry_size = 1;
    }

    insts.push_back(new imulq(make_shared<DollSign>(entry_size * 8), make_shared<Reg>("r8")));
    insts.push_back(new addq(make_shared<Reg>("r9"), make_shared<Reg>("r8")));
    insts.push_back(new movq(make_shared<Reg>("r8"), this->locals[gep->lhs]));
}

void FunctionCG::convertLoadX(vector<XInsts*>& insts, Load* load) {
    insts.push_back(new movq(this->locals[load->src], make_shared<Reg>("r8")));
    insts.push_back(new movq(make_shared<OffReg>("0", make_shared<Reg>("r8")), make_shared<Reg>("r9")));
    insts.push_back(new movq(make_shared<Reg>("r9"), this->locals[load->lhs]));
}

void FunctionCG::convertStoreX(vector<XInsts*>& insts, Store* store) {
    if(store->op->get_type() == "CONST"){
        Const* op = dynamic_cast<Const*>(store->op);
        insts.push_back(new movq(make_shared<DollSign>(op->num), make_shared<Reg>("r8")));
    }
    else{
        Var* op = dynamic_cast<Var*>(store->op);
        insts.push_back(new movq(this->locals[op->id], make_shared<Reg>("r8")));
    }
    insts.push_back(new movq(this->locals[store->dst], make_shared<Reg>("r9")));
    insts.push_back(new movq(make_shared<Reg>("r8"), make_shared<OffReg>("0", make_shared<Reg>("r9"))));
}

void FunctionCG::convertGfpX(vector<XInsts*>& insts, Gfp* gfp, FunctionLIR* flir, ProgramLIR& programLIR) {
    insts.push_back(new movq(this->locals[gfp->src], make_shared<Reg>("r8")));

    shared_ptr<Type> src_type = programLIR.findType(gfp->src, flir);
    PtrType* src = dynamic_cast<PtrType*>(src_type.get());
    if (!src) {
        cout << "error at convertGfpX" << endl;
    }
    
    string structID = src->ref->print_type();
    int index = distance(programLIR.StructsLIR[structID].begin(), programLIR.StructsLIR[structID].find(gfp->field));

    insts.push_back(new leaq(make_shared<OffReg>(std::to_string(index * 8), make_shared<Reg>("r8")), make_shared<Reg>("r9")));
    insts.push_back(new movq(make_shared<Reg>("r9"), this->locals[gfp->lhs]));
}

void FunctionCG::convertX(vector<XInsts*>& insts, shared_ptr<LirInst> instLIR, FunctionLIR* flir, ProgramLIR& programLIR) {
    if (instLIR->get_type() == "COPY") {
        Copy* cpy = dynamic_cast<Copy*>(instLIR.get());
        convertCopyX(insts, cpy);
    }
    else if (instLIR->get_type() == "ARITH") {
        Arith* arith = dynamic_cast<Arith*>(instLIR.get());
        convertArithX(insts, arith);
    }
    else if (instLIR->get_type() == "CMP") {
        Cmp* cmp = dynamic_cast<Cmp*>(instLIR.get());
        convertCmpX(insts, cmp);
    }
    else if (instLIR->get_type() == "CALLEXT") {
        CallExt* callext = dynamic_cast<CallExt*>(instLIR.get());
        convertCallExtX(insts, callext);
    }
    else if (instLIR->get_type() == "CALLDIRECT"){
        CallDirect* calldir = dynamic_cast<CallDirect*>(instLIR.get());
        convertCallDirX(insts, calldir);
    }
    else if (instLIR->get_type() == "ALLOC") {
        Alloc* alloc = dynamic_cast<Alloc*>(instLIR.get());
        convertAllocX(insts, alloc, flir, programLIR);
    }
    else if (instLIR->get_type() == "GEP") {
        Gep* gep = dynamic_cast<Gep*>(instLIR.get());
        convertGepX(insts, gep, flir, programLIR);
    }
    else if (instLIR->get_type() == "LOAD") {
        Load* load = dynamic_cast<Load*>(instLIR.get());
        convertLoadX(insts, load);
    }
    else if (instLIR->get_type() == "STORE") {
        Store* store = dynamic_cast<Store*>(instLIR.get());
        convertStoreX(insts, store);
    }
    else if (instLIR->get_type() == "GFP") {
        Gfp* gfp = dynamic_cast<Gfp*>(instLIR.get());
        convertGfpX(insts, gfp, flir, programLIR);
    }
}

void FunctionCG::convertTermX(vector<XInsts*>& insts, shared_ptr<Terminal> termLIR) {
    if (termLIR->get_type() == "RET") {
        Ret* ret = dynamic_cast<Ret*>(termLIR.get());
        convertRetX(insts, ret);
    }
    else if (termLIR->get_type() == "BRANCH") {
        Branch* branch = dynamic_cast<Branch*>(termLIR.get());
        convertBranchX(insts, branch);
    }
    else if (termLIR->get_type() == "JUMP") {
        Jump* jump = dynamic_cast<Jump*>(termLIR.get()); 
        convertJumpX(insts, jump);
    }
    else if(termLIR->get_type() == "CALLDIRECT"){
        CallDirect* calldir = dynamic_cast<CallDirect*>(termLIR.get());
        convertCallDirX(insts, calldir);
    }
    else if(termLIR->get_type() == "CALLINDIRECT"){
        CallIndirect* callindir = dynamic_cast<CallIndirect*>(termLIR.get());
        convertCallIndirX(insts,callindir);
    }
}

FunctionCG::FunctionCG(FunctionLIR* flir, ProgramLIR& programLIR) {
    string f_name = flir->name;
    shared_ptr<Reg> FP = make_shared<Reg>("rbp");
    shared_ptr<Reg> SP = make_shared<Reg>("rsp");
    shared_ptr<Reg> RIP = make_shared<Reg>("rip");

    
    for(auto glb : programLIR.GlobalsLIR){
        if (glb.second->print_type().substr(0, 2) == "&(") {
            this->locals[glb.first] = make_shared<OffReg>(glb.first + "_", RIP);
        }
        else {
            this->locals[glb.first] = make_shared<OffReg>(glb.first, RIP);
        }
        
    }

    int paramIndex = 1;
    for (auto prm : flir->params) {
        this->locals[prm.first] = make_shared<OffReg>(std::to_string(++paramIndex * 8), FP);
    }
    
    int numLocals = 0;
    for (auto loc : flir->locals) {
        this->locals[loc.first] = make_shared<OffReg>(std::to_string(++numLocals * -8), FP);
    }
    numLocals = (numLocals % 2 == 0) ? numLocals : numLocals + 1; // double alignment

    
    // prologue
    BlockCG* prol = new BlockCG();

    vector<XInsts*> prol_insts;
    prol_insts.push_back(new pushq(FP));
    prol_insts.push_back(new movq(SP, FP));
    prol_insts.push_back(new subq(make_shared<DollSign>(numLocals * 8), SP));
    for (auto loc : flir->locals) {
        prol_insts.push_back(new movq(make_shared<DollSign>(0), this->locals[loc.first]));
    }
    prol_insts.push_back(new jmp(f_name + "_entry"));

    prol->name = f_name;
    prol->insts = prol_insts;

    // epilogue
    BlockCG* epil = new BlockCG();

    vector<XInsts*> epil_insts;
    epil_insts.push_back(new movq(FP, SP));
    epil_insts.push_back(new popq(FP));
    epil_insts.push_back(new ret());

    epil->name = f_name + "_epilogue";
    epil->insts = epil_insts;

    this->name = f_name;
    this->prologue = prol;
    this->epilogue = epil;

    // body
    for (auto block : flir->body) {
        string name = f_name + "_" + block.first;
        BlockCG* body = new BlockCG();
        vector<XInsts*> insts;
        for (auto instLIR : block.second->insts) {
            convertX(insts, instLIR, flir, programLIR);
        }
        convertTermX(insts, block.second->term);
        body->insts = insts;
        body->name = name;
        this->blocks[name] = body;
    }
}
FunctionCG::~FunctionCG() {
    delete this->prologue;
    for (auto pair : this->blocks) {
        delete pair.second;
    }
    delete this->epilogue;
}
string FunctionCG::to_string() const {
    string ret = ".globl " + this->name + "\n";
    ret += this->prologue->to_string();
    for (auto pair : this->blocks) {
        ret += pair.second->to_string();
    }
    ret += this->epilogue->to_string();
    return ret;
}