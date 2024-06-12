#include "objCode.h"

// registers
char *regNames[32] = {
    "$zero",    // 0 : always be constant 0
    "$at",      // 1 : kept by assembler
    "$v0", "$v1",   // 2-3 : values for exprs or funcs
    "$a0", "$a1", "$a2", "$a3",   // 4-7 : arguments of functions
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",     // 8-15 : temporaries
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",     // 16-23 : saved values
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"      // 24-31 
    };

int frameOffset;
struct Register regs[32];
VarDesc varDescHead;

static void printDebug(char* msg)
{
    // printf("In %s!\n", msg);
}

void initRegisters()
{
    printDebug("initRegisters");
    varDescHead = NULL;
    frameOffset = 0;
    for(int i=0; i<32; i++){
        regs[i].valid = 1;
        regs[i].name = regNames[i];
        regs[i].varDesc = NULL;
    }
}

void resetRegisters()
{
    printDebug("resetRegisters");
    for(int i=8; i<=23; i++){
        regs[i].valid = 1;
        regs[i].varDesc = NULL;
    }
}

VarDesc getVarDesc(Operand op)
{
    printDebug("getVarDesc");
    // op->kind == {TEMP_OP, VAR_OP}
    VarDesc temp = varDescHead;
    while(temp!=NULL){
        // printf("here!!!\n");
        if(op->kind == TEMP_OP){
            char test[10];
            sprintf(test, "t%d", op->u.tempNo);
            // printf("test: %s\n", test);
            // printf("temp: %s\n", temp->name);
            if(strcmp(test, temp->name)==0){
                return temp;
            }
        }
        else if(op->kind == VAR_OP){
            char test[10];
            sprintf(test, "v%d", op->u.varNo);
            // printf("test: %s\n", test);
            // printf("temp: %s\n", temp->name);
            if(strcmp(test, temp->name)==0){
                return temp;
            }
        }
        else{
            if(strcmp(op->strval, temp->name)==0){
                return temp;
            }
        }
        // printf("here1\n");
        temp = temp->next;
        // printf("here2\n");
    }
    // printf("here!!!\n");
    return NULL;
}

void createVarDesc(Operand op)
{
    printDebug("createVarDesc");
    if(op->kind == CONS_OP){
        return ;
    }
    // if(op->kind == VAR_OP){
    //     printf("here\n");
    // }
    VarDesc tempOffset = getVarDesc(op);
    if(tempOffset!=NULL) return;

    // create variable offset here
    frameOffset += 4;
    VarDesc newOffset = (VarDesc)malloc(sizeof(struct VarDesc_));
    // can only be TEMP or VAR
    if(op->kind == TEMP_OP){
        sprintf(newOffset->name, "t%d", op->u.tempNo);
    }else{
        // VAR_OP
        sprintf(newOffset->name, "v%d", op->u.varNo);
        // strcpy(newOffset->name, op->strval);
    }
    newOffset->offset = -frameOffset; // off($fp)
    newOffset->next = varDescHead;
    varDescHead = newOffset;
    return ;
}

// go over all regs for an available one
int randd = 8;
int getReg(Operand op, FILE* file){
    printDebug("getReg");
    // static int regNum = 0;
    // regNum++;
    // return regNum;

    // CONSTANT
    // fprintf(file, "here\n");
    if(op->kind == CONS_OP){
        // fprintf(file, "here\n");
        for(int i=8; i<=23; i++){
            if(regs[i].valid == 1){
                regs[i].valid = 0;
                fprintf(file, "\tli %s, %d\n", regs[i].name, op->intval);
                return i;
            }
        }
        saveRegValue(randd, file);
        randd++; 
        if(randd==24) randd=8;
        return getReg(op, file);
    }
    // Others
    for(int i=8; i<=23; i++){
        if(regs[i].valid == 1){
            regs[i].valid = 0;
            regs[i].varDesc = getVarDesc(op);
            // if(regs[i].varDesc==NULL){
            //     // printf()
            //     // printf("here:::%d\n", op->kind);
            // }
            // printf("here\n");
            fprintf(file, "\tlw %s, %d($fp)\n", regs[i].name, regs[i].varDesc->offset);
            return i;
        }
    }
    // printf("here: %d\n", randd);
    saveRegValue(randd, file);
    randd++; 
    if(randd==24) randd=8;
    return getReg(op, file);
}

// save value in reg to memory
void saveRegValue(int idx, FILE* file)
{
    printDebug("saveRegValue");
    int off = regs[idx].varDesc->offset;
    fprintf(file, "\tsw %s, %d($fp)\n", regs[idx].name, off);
    regs[idx].valid = 1;
}

void generateObjectCode(FILE* file)
{
    printDebug("generateObjectCode");
    fprintf(file, INITCODE);
    initRegisters();
    InterCodes temp = interCodeHead->next;
    while(temp != interCodeHead){
        // printf("here: %d\n", temp->code->kind);
        switch(temp->code->kind){
            case LABEL_IC:
            {
                genLabelCode(temp, file);
                break;
            }
            case FUNCTION_IC:
            {
                genFunctionCode(temp, file);
                break;
            }
            case ASSIGN_IC:
            {
                genAssignCode(temp, file);
                break;
            }
            case ADD_IC:
            {
                genAddCode(temp, file);
                break;
            }
            case SUB_IC:
            {
                genSubCode(temp, file);
                break;
            }
            case MUL_IC:
            {
                genMulCode(temp, file);
                break;
            }
            case DIV_IC:
            {
                genDivCode(temp, file);
                break;
            }
            case GET_ADDR_IC:
            {
                genGetAddrCode(temp, file);
                break;
            }
            case GET_CONTENT_IC:
            {
                genGetContentCode(temp, file);
                break;
            }
            case WRITE_ADDR_IC:
            {
                genWriteAddrCode(temp, file);
                break;
            }
            case GOTO_IC:
            {
                genGotoCode(temp, file);
                break;
            }
            case IF_GOTO_IC:
            {
                genIfGotoCode(temp, file);
                break;
            }
            case RETURN_IC:
            {
                genReturnCode(temp, file);
                break;
            }
            case DEC_IC:
            {
                genDecCode(temp, file);
                break;
            }
            case ARG_IC:
            {
                genArgCode(temp, file);
                break;
            }
            case CALL_IC:
            {
                genCallCode(temp, file);
                break;
            }
            case PARAM_IC:
            {
                genParamCode(temp, file);
                break;
            }
            case READ_IC:
            {
                genReadCode(temp, file);
                break;
            }
            case WRITE_IC:
            {
                genWriteCode(temp, file);
                break;
            }
        }
        temp = temp->next;
    }
    // printf("here\n");
}


void genLabelCode(InterCodes temp, FILE* file)
{
    printDebug("genLabelCode");
    fprintf(file, "label%d:\n", temp->code->u.singleOP.op->u.labelNo);
}

void genFunctionCode(InterCodes temp, FILE* file)
{
    printDebug("genFunctionCode");
    fprintf(file, "%s:\n", temp->code->u.singleOP.op->strval);

    // preliminarily create vars and allocate spaces in stack here
    fprintf(file, "\taddi $sp, $sp, -8\n");
    fprintf(file, "\tsw $fp, 0($sp)\n");
    fprintf(file, "\tsw $ra, 4($sp)\n");
    fprintf(file, "\tmove $fp, $sp\n");

    frameOffset = 0;
    int paramCnt = 0;
    InterCodes tempIC = temp->next;
    while(tempIC->code->kind == PARAM_IC){
        VarDesc vo = (VarDesc)malloc(sizeof(struct VarDesc_));
        vo->offset = paramCnt*4 + 8;
        paramCnt++;
        // strcpy(vo->name, tempIC->code->u.singleOP.op->strval);
        sprintf(vo->name, "v%d", tempIC->code->u.singleOP.op->u.varNo);
        vo->next = varDescHead;
        varDescHead = vo;
        tempIC = tempIC->next;
        // printf("%s\n", vo->name);
    }
    // printf("here\n");
    while(tempIC!=interCodeHead && tempIC->code->kind!=FUNCTION_IC){
        switch (tempIC->code->kind)
        {
            // case GET_ADDR_IC:
            // {
            //     // TBD
            //     break;
            // }
            // case GET_CONTENT_IC:
            // {
            //     // x := *y
            //     break;
            // }
            // case WRITE_ADDR_IC:
            // {
            //     // *x := y
            //     break;
            // }
            case GET_ADDR_IC:
            case GET_CONTENT_IC:
            case WRITE_ADDR_IC:
            case ASSIGN_IC:
            {
                createVarDesc(tempIC->code->u.binOP.op1);
                createVarDesc(tempIC->code->u.binOP.op2);
                break;
            }
            case ADD_IC:
            case SUB_IC:
            case MUL_IC:
            case DIV_IC:
            {
                createVarDesc(tempIC->code->u.triOP.op1);
                createVarDesc(tempIC->code->u.triOP.op1);
                createVarDesc(tempIC->code->u.triOP.result);
                break;
            }
            case DEC_IC:
            {
                frameOffset += tempIC->code->u.binOP.op2->intval;
                VarDesc decOff = (VarDesc)malloc(sizeof(struct VarDesc_));
                decOff->offset = -frameOffset;
                strcpy(decOff->name, tempIC->code->u.binOP.op1->strval);
                decOff->next = varDescHead;
                varDescHead = decOff;
                break;
            }
            case IF_GOTO_IC:
            {
                createVarDesc(tempIC->code->u.jumpOP.op1);
                createVarDesc(tempIC->code->u.jumpOP.op2);
                break;
            }
            case CALL_IC:
            {
                createVarDesc(tempIC->code->u.binOP.op1);
                createVarDesc(tempIC->code->u.binOP.op2);
                break;
            }
            case ARG_IC:
            case WRITE_IC:
            case READ_IC:
            {
                createVarDesc(tempIC->code->u.singleOP.op);
                break;
            }
            default:
            {
                break;
            }
        }
        // printf("%d\n", tempIC->code->kind);
        tempIC = tempIC->next;
    }
    // if(tempIC==NULL) printf("here!!!!!!!############\n");
    // move $sp register
    fprintf(file, "\taddi $sp, $sp, %d\n", -frameOffset);
    resetRegisters();
}

void genAssignCode(InterCodes temp, FILE* file)
{
    printDebug("genAssignCode");
    // x := #k
    // x := y
    Operand op1 = temp->code->u.binOP.op1;
    Operand op2 = temp->code->u.binOP.op2;
    // if(op2->kind == CONS_OP){
    //     int regID = getReg(op1, file);
    //     // int regConst = getReg(op2, file);
    //     fprintf(file, "\tli %s, %d\n", regs[regID].name, op2->intval);
    //     saveRegValue(regID, file);
    // }else{
    //     int regx = getReg(op1, file);
    //     int regy = getReg(op2, file);
    //     fprintf(file, "\tmove %s, %s\n", regs[regx].name, regs[regy].name);
    //     saveRegValue(regx, file);
    // }
    int regx = getReg(op1, file);
    int regy = getReg(op2, file);
    // printf("here\n");
    fprintf(file, "\tmove %s, %s\n", regs[regx].name, regs[regy].name);
    // printf("here\n");
    saveRegValue(regx, file);
}

void genAddCode(InterCodes temp, FILE* file)
{
    printDebug("genAddCode");
    // x := y + #k
    // x := y + z
    Operand resop = temp->code->u.triOP.result;
    Operand op1 = temp->code->u.triOP.op1;
    Operand op2 = temp->code->u.triOP.op2;
    if(op2->kind == CONS_OP){
        int regx = getReg(resop, file);
        int regy = getReg(op1, file);
        fprintf(file, "\taddi %s, %s, %d\n", regs[regx].name, regs[regy].name, op2->intval);
        saveRegValue(regx, file);
    }else{
        int regx = getReg(resop, file);
        int regy = getReg(op1, file);
        int regz = getReg(op2, file);
        fprintf(file, "\tadd %s, %s, %s\n", regs[regx].name, regs[regy].name, regs[regz].name);
        saveRegValue(regx, file);
    }
}

void genSubCode(InterCodes temp, FILE* file)
{
    printDebug("genSubCode");
    // x := y - #k
    // x := y - z
    Operand resop = temp->code->u.triOP.result;
    Operand op1 = temp->code->u.triOP.op1;
    Operand op2 = temp->code->u.triOP.op2;
    if(op2->kind == CONS_OP){
        int regx = getReg(resop, file);
        int regy = getReg(op1, file);
        fprintf(file, "\taddi %s, %s, %d\n", regs[regx].name, regs[regy].name, -op2->intval);
        saveRegValue(regx, file);
    }else{
        int regx = getReg(resop, file);
        int regy = getReg(op1, file);
        int regz = getReg(op2, file);
        fprintf(file, "\tsub %s, %s, %s\n", regs[regx].name, regs[regy].name, regs[regz].name);
        saveRegValue(regx, file);
    }
}

void genMulCode(InterCodes temp, FILE* file)
{
    printDebug("genMulCode");
    // x := y * z
    Operand resop = temp->code->u.triOP.result;
    Operand op1 = temp->code->u.triOP.op1;
    Operand op2 = temp->code->u.triOP.op2;
    int regx = getReg(resop, file);
    int regy = getReg(op1, file);
    int regz = getReg(op2, file);
    fprintf(file, "\tmul %s, %s, %s\n", regs[regx].name, regs[regy].name, regs[regz].name);
    saveRegValue(regx, file);
}

void genDivCode(InterCodes temp, FILE* file)
{
    printDebug("genDivCode");
    // x := y / z
    Operand resop = temp->code->u.triOP.result;
    Operand op1 = temp->code->u.triOP.op1;
    Operand op2 = temp->code->u.triOP.op2;
    int regx = getReg(resop, file);
    int regy = getReg(op1, file);
    int regz = getReg(op2, file);
    fprintf(file, "\tdiv %s, %s, %s\n", regs[regx].name, regs[regy].name, regs[regz].name);
    fprintf(file, "\tmflo %s\n", regs[regx].name);
    saveRegValue(regx, file);
}

// there is no get addr IR
void genGetAddrCode(InterCodes temp, FILE* file)
{
    printDebug("genGetAddrCode");
}

void genGetContentCode(InterCodes temp, FILE* file)
{
    printDebug("genGetContentCode");
    // x := *y
    Operand op1 = temp->code->u.binOP.op1;
    Operand op2 = temp->code->u.binOP.op2;
    int regx = getReg(op1, file);
    int regy = getReg(op2, file);
    fprintf(file, "\tlw %s, 0(%s)\n", regs[regx].name, regs[regy].name);
    saveRegValue(regx, file);
}

void genWriteAddrCode(InterCodes temp, FILE* file)
{
    printDebug("genWriteAddrCode");
    // *x := y
    Operand op1 = temp->code->u.binOP.op1;
    Operand op2 = temp->code->u.binOP.op2;
    int regx = getReg(op1, file);
    int regy = getReg(op2, file);
    fprintf(file, "\tsw %s, 0(%s)\n", regs[regy].name, regs[regx].name);
}

void genGotoCode(InterCodes temp, FILE* file)
{
    printDebug("genGotoCode");
    fprintf(file, "\tj label%d\n", temp->code->u.singleOP.op->u.labelNo);
}

void genIfGotoCode(InterCodes temp, FILE* file)
{
    printDebug("genIfGotoCode");
    char* relop = temp->code->u.jumpOP.relop;
    Operand op1 = temp->code->u.jumpOP.op1;
    Operand op2 = temp->code->u.jumpOP.op2;
    int regx = getReg(op1, file);
    int regy = getReg(op2, file);
    if(strcmp(relop, "==")==0){
        fprintf(file, "\tbeq %s, %s, label%d\n", regs[regx].name, regs[regy].name, temp->code->u.jumpOP.label->u.labelNo);
    }else if(strcmp(relop, "!=")==0){
        fprintf(file, "\tbne %s, %s, label%d\n", regs[regx].name, regs[regy].name, temp->code->u.jumpOP.label->u.labelNo);
    }else if(strcmp(relop, ">")==0){
        fprintf(file, "\tbgt %s, %s, label%d\n", regs[regx].name, regs[regy].name, temp->code->u.jumpOP.label->u.labelNo);
    }else if(strcmp(relop, "<")==0){
        fprintf(file, "\tblt %s, %s, label%d\n", regs[regx].name, regs[regy].name, temp->code->u.jumpOP.label->u.labelNo);
    }else if(strcmp(relop, ">=")==0){
        fprintf(file, "\tbge %s, %s, label%d\n", regs[regx].name, regs[regy].name, temp->code->u.jumpOP.label->u.labelNo);
    }else if(strcmp(relop, "<=")==0){
        fprintf(file, "\tble %s, %s, label%d\n", regs[regx].name, regs[regy].name, temp->code->u.jumpOP.label->u.labelNo);
    }
}

void genReturnCode(InterCodes temp, FILE* file)
{
    printDebug("genReturnCode");
    fprintf(file, "\tlw $ra, 4($fp)\n");
    fprintf(file, "\taddi $sp, $fp, 8\n");
    int idx = getReg(temp->code->u.singleOP.op, file);
    fprintf(file, "\tlw $fp, 0($fp)\n");
    fprintf(file, "\tmove $v0, %s\n", regs[idx].name);
    fprintf(file, "\tjr $ra\n");
    resetRegisters();
}

// done in genFunctionCode()
void genDecCode(InterCodes temp, FILE* file)
{
    printDebug("genDecCode");
}

void genArgCode(InterCodes temp, FILE* file)
{
    printDebug("genArgCode");
    int argCnt = 0;
    InterCodes tempIC = temp;
    while(tempIC->code->kind == ARG_IC){
        // push argument into stack
        // not using registers $a0-$a3
        argCnt++;
        fprintf(file, "\taddi $sp, $sp, -4\n");
        int idx = getReg(tempIC->code->u.singleOP.op, file);
        fprintf(file, "\tsw %s, 0($sp)\n", regs[idx].name);
        regs[idx].valid = 1;
        tempIC = tempIC->next;
    }
    // assert(tempIC->code->kind == CALL_IC);
    Operand op1 = tempIC->code->u.binOP.op1;
    Operand op2 = tempIC->code->u.binOP.op2;
    fprintf(file, "\tjal %s\n", op2->strval);
    fprintf(file, "\taddi $sp, $sp, %d\n", 4*argCnt);
    int idx = getReg(op1, file);
    // printf("here!!!\n");
    fprintf(file, "\tmove %s, $v0\n", regs[idx].name);
    saveRegValue(idx, file);
}

// done in genArgCode()
void genCallCode(InterCodes temp, FILE* file)
{
    printDebug("genCallCode");
}

// done in genFunctionCode()
void genParamCode(InterCodes temp, FILE* file)
{
    printDebug("genParamCode");
}

void genReadCode(InterCodes temp, FILE* file)
{
    printDebug("genReadCode");
    fprintf(file, "\taddi $sp, $sp, -4\n");
    fprintf(file, "\tsw $ra, 0($sp)\n");
    fprintf(file, "\tjal read\n");
    fprintf(file, "\tlw $ra, 0($sp)\n");
    fprintf(file, "\taddi $sp, $sp, 4\n");
    int regID = getReg(temp->code->u.singleOP.op, file);
    fprintf(file, "\tmove %s, $v0\n", regs[regID].name);
    saveRegValue(regID, file);
}

void genWriteCode(InterCodes temp, FILE* file)
{
    printDebug("genWriteCode");
    int regID = getReg(temp->code->u.singleOP.op, file);
    fprintf(file, "\tmove $a0, %s\n", regs[regID].name);
    fprintf(file, "\taddi $sp, $sp, -4\n");
    fprintf(file, "\tsw $ra, 0($sp)\n");
    fprintf(file, "\tjal write\n");
    fprintf(file, "\tlw $ra, 0($sp)\n");
    fprintf(file, "\taddi $sp, $sp, 4\n");
}
