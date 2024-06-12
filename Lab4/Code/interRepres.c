#include "interRepres.h"

InterCodes interCodeHead = NULL;
InterCodes interCodeTail = NULL;

int varNo = 1;
int tempNo = 1;
int labelNo = 1;
int cannotTranslate;

void highDimArrayError()
{
    if(cannotTranslate==0){
        printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
        cannotTranslate = 1;
    }
}

static void printDebug(char* msg)
{
    // printf("In %s!\n", msg);
}

void insertInterCode(int kind, Operand op1, Operand op2, Operand result, char* operator)
{
    // printDebug("insertInterCode");
    InterCodes ics = (InterCodes)malloc(sizeof(struct InterCodes_));
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    ic->kind = kind;
    ics->code = ic;
    ics->next = NULL;
    ics->prev = NULL;
    switch(kind){
        case LABEL_IC:
        case FUNCTION_IC:
        case GOTO_IC:
        case RETURN_IC:
        case ARG_IC:
        case PARAM_IC:
        case READ_IC:
        case WRITE_IC:
        {
            // single OP
            ic->u.singleOP.op = op1;
            break;
        }
        case ASSIGN_IC:
        case GET_ADDR_IC:
        case GET_CONTENT_IC:
        case WRITE_ADDR_IC:
        case DEC_IC:
        case CALL_IC:
        {
            // bin OP
            ic->u.binOP.op1 = op1;
            ic->u.binOP.op2 = op2;
            break;
        }
        case ADD_IC:
        case SUB_IC:
        case MUL_IC:
        case DIV_IC:
        {
            // tri OP
            ic->u.triOP.op1 = op1;
            ic->u.triOP.op2 = op2;
            ic->u.triOP.result = result;
            break;
        }
        case IF_GOTO_IC:
        {
            // jump OP
            ic->u.jumpOP.op1 = op1;
            ic->u.jumpOP.op2 = op2;
            ic->u.jumpOP.label = result;
            ic->u.jumpOP.relop = operator;
            break;
        }
    }
    ics->prev = interCodeTail;
    ics->next = interCodeHead;
    interCodeTail->next = ics;
    interCodeHead->prev = ics;
    interCodeTail = ics;
}

Operand createOperand(int kind, int size, char* name)
{
    /*
        kind:
            CONS_OP:
                name == NULL: val = size
                name != NULL:
                    size == 0 : int
                    size == 1 : float
            VAR_OP: name
            FUNC_OP: name
            LABEL_OP
    */
    // printDebug("createOperand");
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    op->size = size;
    op->strval = name;
    op->isAddr = 0;
    op->isStruct = 0;
    op->isArray = 0;
    op->type = NULL;
    switch(kind){
        case VAR_OP:{
            // printf("In create\n");
            // printf("name in create: %s\n", name);
            for(int i=1; i<varNo; i++){
                if(strcmp(VAR_Operands[i]->strval, name)==0){
                    // printf("Match %s!\n", name);
                    // free(op);
                    // return VAR_Operands[i];

                    // make a copy and return
                    op->kind = VAR_Operands[i]->kind;
                    op->size = VAR_Operands[i]->size;
                    op->strval = VAR_Operands[i]->strval;
                    op->isAddr = VAR_Operands[i]->isAddr;
                    op->isStruct = VAR_Operands[i]->isStruct;
                    op->isArray = VAR_Operands[i]->isArray;
                    op->type = VAR_Operands[i]->type;
                    op->u.varNo = VAR_Operands[i]->u.varNo;
                    return op;
                }
                // if(i<=3)printf("Not match name %d: %s\n", i, VAR_Operands[i]->strval);
            }
            op->u.varNo = varNo;
            VAR_Operands[varNo++] = op;
            // printf("creating new op: %s\n", name);
            break;
        }
        case CONS_OP: {
            if(name==NULL){
                // val = size
                op->intval = size;
            }else{
                // val = atoi(name)
                // size = 0: int
                // size = 1: float
                if(size==0){
                    // int
                    op->intval = atoi(name);
                }else{
                    // float
                    op->floatval = atof(name);
                }
            }
            break;
        }
        case TEMP_OP: {
            op->u.tempNo = tempNo++;
            break;
        }
        case FUNC_OP: {
            break;
        }
        case LABEL_OP: {
            op->u.labelNo = labelNo++;
            break;
        }
    }
    return op;
}

void printOperand(Operand op, FILE* file)
{
    // printf("Operand: %s\n", op->strval);
    // printDebug("printOperand");
    switch(op->kind){
        case VAR_OP:
        {
            fprintf(file, "v%d", op->u.varNo);
            break;
        }
        case CONS_OP:
        {
            if(op->strval==NULL){
                fprintf(file, "#%d", op->intval);
            }else{
                if(op->size==0){
                    //int
                    fprintf(file, "#%d", op->intval);
                }else{
                    fprintf(file, "#%.6f", op->floatval);
                }
            }
            break;
        }
        case TEMP_OP:
        {
            fprintf(file, "t%d", op->u.tempNo);
            break;
        }
        case FUNC_OP:
        {
            fprintf(file, "%s", op->strval);
            break;
        }
        case LABEL_OP:
        {
            fprintf(file, "label%d", op->u.labelNo);
            break;
        }
    }
}

void printInterCode(FILE* file)
{
    printDebug("printInterCode");
    if(cannotTranslate==1){
        // fprintf(file, "Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.");
        return;
    }
    InterCodes temp = interCodeHead->next;
    while(temp != interCodeHead){
        // printf("here: %d\n", temp->code->kind);
        switch(temp->code->kind){
            case LABEL_IC:
            {
                fprintf(file, "LABEL label%d : \n", temp->code->u.singleOP.op->u.labelNo);
                break;
            }
            case FUNCTION_IC:
            {
                fprintf(file, "FUNCTION %s : \n", temp->code->u.singleOP.op->strval);
                break;
            }
            case ASSIGN_IC:
            {
                printOperand(temp->code->u.binOP.op1, file);
                fprintf(file, " := ");
                printOperand(temp->code->u.binOP.op2, file);
                fprintf(file, "\n");
                break;
            }
            case ADD_IC:
            {
                printOperand(temp->code->u.triOP.result, file);
                fprintf(file, " := ");
                printOperand(temp->code->u.triOP.op1, file);
                fprintf(file, " + ");
                printOperand(temp->code->u.triOP.op2, file);
                fprintf(file, "\n");
                break;
            }
            case SUB_IC:
            {
                // fclose(file);
                // if(temp->code->u.triOP.op1!=NULL)
                //     printf("%d\n", temp->code->u.triOP.op1->kind);
                // printf("here\n");
                printOperand(temp->code->u.triOP.result, file);
                fprintf(file, " := ");
                printOperand(temp->code->u.triOP.op1, file);
                fprintf(file, " - ");
                printOperand(temp->code->u.triOP.op2, file);
                fprintf(file, "\n");
                break;
            }
            case MUL_IC:
            {
                printOperand(temp->code->u.triOP.result, file);
                fprintf(file, " := ");
                printOperand(temp->code->u.triOP.op1, file);
                fprintf(file, " * ");
                printOperand(temp->code->u.triOP.op2, file);
                fprintf(file, "\n");
                break;
            }
            case DIV_IC:
            {
                printOperand(temp->code->u.triOP.result, file);
                fprintf(file, " := ");
                printOperand(temp->code->u.triOP.op1, file);
                fprintf(file, " / ");
                printOperand(temp->code->u.triOP.op2, file);
                fprintf(file, "\n");
                break;
            }
            case GET_ADDR_IC:
            {
                printOperand(temp->code->u.binOP.op1, file);
                fprintf(file, " := &");
                printOperand(temp->code->u.binOP.op2, file);
                fprintf(file, "\n");
                break;
            }
            case GET_CONTENT_IC:
            {
                printOperand(temp->code->u.binOP.op1, file);
                fprintf(file, " := *");
                printOperand(temp->code->u.binOP.op2, file);
                fprintf(file, "\n");
                break;
            }
            case WRITE_ADDR_IC:
            {
                fprintf(file, "*");
                printOperand(temp->code->u.binOP.op1, file);
                fprintf(file, " := ");
                printOperand(temp->code->u.binOP.op2, file);
                fprintf(file, "\n");
                break;
            }
            case GOTO_IC:
            {
                fprintf(file, "GOTO label%d\n", temp->code->u.singleOP.op->u.labelNo);
                break;
            }
            case IF_GOTO_IC:
            {
                fprintf(file, "IF ");
                printOperand(temp->code->u.jumpOP.op1, file);
                fprintf(file, " %s ", temp->code->u.jumpOP.relop);
                printOperand(temp->code->u.jumpOP.op2, file);
                // fclose(file);
                fprintf(file, " GOTO label%d\n", temp->code->u.jumpOP.label->u.labelNo);
                // printf("here!!!!!!!!\n");
                break;
            }
            case RETURN_IC:
            {
                fprintf(file, "RETURN ");
                printOperand(temp->code->u.singleOP.op, file);
                fprintf(file, "\n");
                break;
            }
            case DEC_IC:
            {
                fprintf(file, "DEC ");
                printOperand(temp->code->u.binOP.op1, file);
                fprintf(file, " %d\n", temp->code->u.binOP.op2->intval);
                // printOperand(temp->code->u.binOP.op2, file);
                break;
            }
            case ARG_IC:
            {
                fprintf(file, "ARG ");
                printOperand(temp->code->u.singleOP.op, file);
                fprintf(file, "\n");
                break;
            }
            case CALL_IC:
            {
                printOperand(temp->code->u.binOP.op1, file);
                fprintf(file, " := CALL ");
                printOperand(temp->code->u.binOP.op2, file);
                fprintf(file, "\n");
                break;
            }
            case PARAM_IC:
            {
                fprintf(file, "PARAM ");
                printOperand(temp->code->u.singleOP.op, file);
                fprintf(file, "\n");
                break;
            }
            case READ_IC:
            {
                fprintf(file, "READ ");
                printOperand(temp->code->u.singleOP.op, file);
                fprintf(file, "\n");
                break;
            }
            case WRITE_IC:
            {
                fprintf(file, "WRITE ");
                printOperand(temp->code->u.singleOP.op, file);
                fprintf(file, "\n");
                break;
            }
        }
        temp = temp->next;
    }
}

static NODE getChild(NODE node, int n)
{
    if(node==NULL) return NULL;
    NODE t = node->child;
    if(t==NULL){return t;}
    else{
        for(int i=1; i<=n; i++){
            t = t->sibling;
            if(t==NULL) return t;
        }
        return t;
    }
}

void generateIR(NODE root, FILE* file)
{
    printDebug("generateIR");
    if(root==NULL) return;
    // init
    interCodeHead = (InterCodes)malloc(sizeof(struct InterCodes_));
    interCodeHead->prev = NULL;
    interCodeHead->next = NULL;
    interCodeTail = interCodeHead;

    // generate IR code
    Program_IR(root);

    // print IR code
    // printInterCode(file);
}

void Program_IR(NODE node)
{
    /*
        Program : ExtDefList
            ;
    */
    printDebug("Program_IR");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    if(ch0!=NULL) ExtDefList_IR(ch0);
}

void ExtDefList_IR(NODE node)
{
    /*
        ExtDefList : ExtDef ExtDefList
            |                               
            ;
    */
    printDebug("ExtDefList_IR");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    // printf("here\n");
    if(ch0!=NULL){
        ExtDef_IR(ch0);
    }
    if(ch1!=NULL){
        ExtDefList_IR(ch1);
    }
}

void ExtDef_IR(NODE node)
{
    /*
        ExtDef : Specifier ExtDecList SEMI  {$$=nonTerminalNode("ExtDef", 3, $1, $2, $3);}
            | Specifier SEMI                {$$=nonTerminalNode("ExtDef", 2, $1, $2);}
            | Specifier FunDec CompSt       {$$=nonTerminalNode("ExtDef", 3, $1, $2, $3);}
            ;
    */
    printDebug("ExtDef_IR");
    if(node==NULL) return;
    NODE ch1 = getChild(node, 1);
    NODE ch2 = getChild(node, 2);
    if(strcmp(ch1->name, "ExtDecList")==0){
        ExtDecList_IR(ch1);
    }else if(strcmp(ch1->name, "FunDec")==0){
        FunDec_IR(ch1);
        CompSt_IR(ch2);
    }
}

// void Specifier_IR(NODE node);
// void StructSpecifier_IR(NODE node);
// void OptTag_IR(NODE node);

void DefList_IR(NODE node)
{
    /*
        DefList : Def DefList               {$$ = nonTerminalNode("DefList", 2, $1, $2);}
            |                               {$$=NULL;epsilon("DefList");}
            ;
    */
    printDebug("DefList_IR");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    if(ch0!=NULL){
        Def_IR(ch0);
    }
    if(ch1!=NULL){
        DefList_IR(ch1);
    }
}

void Def_IR(NODE node)
{
    /*
        Def : Specifier DecList SEMI        {$$ = nonTerminalNode("Def", 3, $1, $2, $3);}
            ;
    */
    printDebug("Def_IR");
    if(node==NULL) return;
    NODE ch1 = getChild(node, 1);
    if(ch1!=NULL){
        DecList_IR(ch1);
    }
}

void DecList_IR(NODE node)
{
    /*
        DecList : Dec                       {$$ = nonTerminalNode("DecList", 1, $1);}
            | Dec COMMA DecList             {$$ = nonTerminalNode("DecList", 3, $1, $2, $3);}
            ;
    */
    printDebug("DecList_IR");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    if(ch0!=NULL){
        Dec_IR(ch0);
    }
    if(ch2!=NULL){
        DecList_IR(ch2);
    }
}

void Dec_IR(NODE node)
{
    /*
        Dec : VarDec                        {$$ = nonTerminalNode("Dec", 1, $1);}
            | VarDec ASSIGNOP Exp           {$$ = nonTerminalNode("Dec", 3, $1, $2, $3);}
            ;
    */
    printDebug("Dec_IR");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    Operand op1=NULL, op2=NULL;
    if(ch0!=NULL && ch2==NULL){
        VarDec_IR(ch0, 0);
    }
    if(ch0!=NULL && ch2!=NULL){
        op1 = VarDec_IR(ch0, 0);
        op2 = Exp_IR(ch2);
        if(op2==NULL) return;
        if(op2->isAddr == 1){
            Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
            insertInterCode(GET_CONTENT_IC, temp4content, op2, NULL, NULL);
            op2 = temp4content;
        }
        insertInterCode(ASSIGN_IC, op1, op2, NULL, NULL);
    }
}

// void Tag_IR(NODE node);

void ExtDecList_IR(NODE node)
{
    /*
        ExtDecList : VarDec                 {$$=nonTerminalNode("ExtDecList", 1, $1);}
            | VarDec COMMA ExtDecList       {$$=nonTerminalNode("ExtDecList", 3, $1, $2, $3);}
            ;
    */
    printDebug("ExtDecList_IR");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    if(ch0!=NULL){
        VarDec_IR(ch0, 0);
    }
    if(ch2!=NULL){
        ExtDecList_IR(ch2);
    }
}

Operand VarDec_IR(NODE node, int param)
{
    /*
        VarDec : ID                         {$$=nonTerminalNode("VarDec", 1, $1);}
            | VarDec LB INT RB              {$$=nonTerminalNode("VarDec", 4, $1, $2, $3, $4);}
            ;
    */
    printDebug("VarDec_IR");
    if(node==NULL) return NULL;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    if(ch0!=NULL && (strcmp(ch0->name, "ID")==0)){
        FieldList field = lookUp(ch0->strval);
        Type type = NULL;
        if(field!=NULL){
            type = field->type;
        }
        if(type!=NULL){
            if(type->kind==STRUCTURE && param==0){
                int size = getTypeSize(type);
                Operand op1 = createOperand(VAR_OP, 0, ch0->strval);
                Operand op2 = createOperand(CONS_OP, size, NULL);
                insertInterCode(DEC_IC, op1, op2, NULL, NULL);
                op1->isStruct = 1;
                return op1;
            }
            else{
                Operand op1 = createOperand(VAR_OP, 0, ch0->strval);
                // printf("This op: %s\n", ch0->strval);
                if(param==1){
                    insertInterCode(PARAM_IC, op1, NULL, NULL, NULL);
                    if(type->kind==STRUCTURE){
                        op1->isAddr = 1;
                    }
                }
                return op1;
            }
        }
    }else if(ch2!=NULL){
        if(param==1){
            highDimArrayError();
        }
        // only 1-d array
        NODE ch0ch0 = getChild(ch0, 0);
        if(ch0ch0!=NULL && (strcmp(ch0ch0->name, "ID")==0)){
            // ID [ INT ]
            int intval = ch2->intval;
            Operand op1 = createOperand(VAR_OP, 0, ch0ch0->strval);
            FieldList field = lookUp(ch0ch0->strval);
            // printf("%s\n", ch0ch0->name);
            if(field!=NULL){
                // printf("here\n");
                Type type = field->type;
                if(type!=NULL){
                    int elemSize = getTypeSize(type);
                    Operand op2 = createOperand(CONS_OP, elemSize*intval, NULL);
                    insertInterCode(DEC_IC, op1, op2, NULL, NULL);
                }
            }
            op1->isArray = 1;
            return op1;
        }else{
            highDimArrayError();
        }
    }
    return NULL;
}

void FunDec_IR(NODE node)
{
    /*
        FunDec : ID LP VarList RP           {$$=nonTerminalNode("FunDec", 4, $1, $2, $3, $4);}
            | ID LP RP                      {$$=nonTerminalNode("FunDec", 3, $1, $2, $3);}
            ;
    */
    printDebug("FunDec_IR");    
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    if(strcmp(ch2->name, "RP")==0){
        // ID LP RP
        Operand op1 = createOperand(FUNC_OP, 0, ch0->strval);
        insertInterCode(FUNCTION_IC, op1, NULL, NULL, NULL);
    }else{
        // ID LP VarList RP
        Operand op1 = createOperand(FUNC_OP, 0, ch0->strval);
        insertInterCode(FUNCTION_IC, op1, NULL, NULL, NULL);
        VarList_IR(ch2);
    }
}

void VarList_IR(NODE node)
{
    /*
        VarList : ParamDec COMMA VarList    {$$=nonTerminalNode("VarList", 3, $1, $2, $3);}
            | ParamDec                      {$$=nonTerminalNode("VarList", 1, $1);}
            ;
    */
    printDebug("VarList_IR");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    if(ch0!=NULL) ParamDec_IR(ch0);
    if(ch2!=NULL) VarList_IR(ch2);
}

void ParamDec_IR(NODE node)
{
    /*
        ParamDec : Specifier VarDec         {$$=nonTerminalNode("ParamDec", 2, $1, $2);}
            ;
    */
    printDebug("ParamDec_IR");
    if(node==NULL) return;
    NODE ch1 = getChild(node, 1);
    if(ch1!=NULL) VarDec_IR(ch1, 1);
}

void CompSt_IR(NODE node)
{
    /*
        CompSt : LC DefList StmtList RC     {$$=nonTerminalNode("CompSt", 4, $1, $2, $3, $4);}
            ;
    */
    printDebug("CompSt_IR");
    if(node==NULL) return;
    NODE ch1 = getChild(node, 1);
    NODE ch2 = getChild(node, 2);
    if(strcmp(ch1->name, "DefList")==0){
        DefList_IR(ch1);
        StmtList_IR(ch2);
    }else if(strcmp(ch1->name, "StmtList")==0){
        StmtList_IR(ch1);
    }
}

void StmtList_IR(NODE node)
{
    /*
        StmtList : Stmt StmtList            {$$=nonTerminalNode("StmtList", 2, $1, $2);}
            |                               {$$=NULL; epsilon("StmtList");}
            ;
    */
    printDebug("StmtList_IR");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    if(ch0!=NULL) Stmt_IR(ch0);
    if(ch1!=NULL) StmtList_IR(ch1);
}

void Stmt_IR(NODE node)
{
    /*
    Stmt : Exp SEMI                     {$$=nonTerminalNode("Stmt", 2, $1, $2);}
        | CompSt                        {$$=nonTerminalNode("Stmt", 1, $1);}
        | RETURN Exp SEMI               {$$=nonTerminalNode("Stmt", 3, $1, $2, $3);}
        | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   {$$ = nonTerminalNode("Stmt", 5, $1, $2, $3, $4, $5);}
        | IF LP Exp RP Stmt ELSE Stmt   {$$ = nonTerminalNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
        | WHILE LP Exp RP Stmt          {$$ = nonTerminalNode("Stmt", 5, $1, $2, $3, $4, $5);}
        ;
    */
    printDebug("Stmt_IR");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    NODE ch2 = getChild(node, 2);
    NODE ch3 = getChild(node, 3);
    NODE ch4 = getChild(node, 4);
    NODE ch6 = getChild(node, 6);
    if(ch1==NULL){
        // CompSt
        CompSt_IR(ch0);
    }else if(ch2==NULL){
        // Exp SEMI
        Exp_IR(ch0);
    }else if(ch3==NULL){
        // RETURN Exp SEMI
        Operand op1 = Exp_IR(ch1);
        if(op1==NULL)return;
        if(op1->kind == CONS_OP){
            Operand temp = createOperand(TEMP_OP, NULL, NULL);
            insertInterCode(ASSIGN_IC, temp, op1, NULL, NULL);
            op1 = temp;
        }
        insertInterCode(RETURN_IC, op1, NULL, NULL, NULL);
    }else if(ch6!=NULL){
        // IF LP Exp RP Stmt ELSE Stmt
        Operand label1 = createOperand(LABEL_OP, NULL, NULL);   //false
        Operand label2 = createOperand(LABEL_OP, NULL, NULL);   //follow
        Operand labelTrue = createOperand(LABEL_OP, NULL, NULL); 
        Condition_IR(ch2, labelTrue, label1);
        insertInterCode(LABEL_IC, labelTrue, NULL, NULL, NULL);
        // insertInterCode(labelTrue, label1, NULL, NULL, NULL);
        Stmt_IR(ch4);
        insertInterCode(GOTO_IC, label2, NULL, NULL, NULL);
        insertInterCode(LABEL_IC, label1, NULL, NULL, NULL);
        Stmt_IR(ch6);
        insertInterCode(LABEL_IC, label2, NULL, NULL, NULL);
    }else{
        if(strcmp(ch0->strval, "while")==0){
            // WHILE LP Exp RP Stmt
            Operand label1 = createOperand(LABEL_OP, NULL, NULL);
            Operand label2 = createOperand(LABEL_OP, NULL, NULL);
            insertInterCode(LABEL_IC, label1, NULL, NULL, NULL);  // go back for another loop check
            Condition_IR(ch2, NULL, label2);
            Stmt_IR(ch4);
            insertInterCode(GOTO_IC, label1, NULL, NULL, NULL);
            insertInterCode(LABEL_IC, label2, NULL, NULL, NULL);
        }else{
            // IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
            Operand label1 = createOperand(LABEL_OP, NULL, NULL);   //false
            Operand label2 = createOperand(LABEL_OP, NULL, NULL);   //true
            Condition_IR(ch2, label2, label1);
            insertInterCode(LABEL_IC, label2, NULL, NULL, NULL);
            Stmt_IR(ch4);
            insertInterCode(LABEL_IC, label1, NULL, NULL, NULL);
        }
    }
}

Operand Exp_IR(NODE node)
{
    /*
    Exp : Exp ASSIGNOP Exp              {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp AND Exp                   {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp OR Exp                    {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp RELOP Exp                 {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp PLUS Exp                  {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp MINUS Exp                 {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp STAR Exp                  {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp DIV Exp                   {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | LP Exp RP                     {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | MINUS Exp                     {$$ = nonTerminalNode("Exp", 2, $1, $2);}
        | NOT Exp                       {$$ = nonTerminalNode("Exp", 2, $1, $2);}
        | ID LP Args RP                 {$$ = nonTerminalNode("Exp", 4, $1, $2, $3, $4);}
        | ID LP RP                      {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp LB Exp RB                 {$$ = nonTerminalNode("Exp", 4, $1, $2, $3, $4);}
        | Exp DOT ID                    {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | ID                            {$$ = nonTerminalNode("Exp", 1, $1);}
        | INT                           {$$ = nonTerminalNode("Exp", 1, $1);}
        | FLOAT                         {$$ = nonTerminalNode("Exp", 1, $1);}
        ;
    */
    printDebug("Exp_IR");
    if(node==NULL)return NULL;
    // printf("%s\n", node->name);
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    NODE ch2 = getChild(node, 2);
    NODE ch3 = getChild(node, 3);
    // if(ch0!=NULL)printf("ch0: %s\n", ch0->strval);
    // if(ch1!=NULL)printf("ch1: %s\n", ch1->strval);
    // if(ch2!=NULL)printf("ch2: %s\n", ch2->strval);
    // if(ch3!=NULL)printf("ch3: %s\n", ch3->strval);
    Operand op=NULL;
    if(ch1==NULL){
        // ID INT FLOAT
        if(ch0->type==ID_){
            FieldList field = lookUp(ch0->strval);
            if(field!=NULL){
                // printf("%s\n", ch0->strval);
                op = createOperand(VAR_OP, NULL, ch0->strval);
            }
        }else{
            if(ch0->type==FLOAT_){
                op = createOperand(CONS_OP, 1, ch0->strval);
            }else if(ch0->type==INT_){
                op = createOperand(CONS_OP, 0, ch0->strval);
            }
        } 
    }else if(ch2==NULL){
        // MINUS Exp
        // NOT Exp
        if(strcmp(ch0->name, "NOT")==0){
            Operand label1 = createOperand(LABEL_OP, NULL, NULL);
            Operand label2 = createOperand(LABEL_OP, NULL, NULL);
            op = createOperand(TEMP_OP, NULL, NULL);
            Operand zero = createOperand(CONS_OP, 0, NULL);
            Operand one = createOperand(CONS_OP, 1, NULL);
            insertInterCode(ASSIGN_IC, op, zero, NULL, NULL);
            Condition_IR(ch1, label1, label2);
            insertInterCode(LABEL_IC, label1, NULL, NULL, NULL);
            insertInterCode(ASSIGN_IC, op, one, NULL, NULL);
            insertInterCode(LABEL_IC, label2, NULL, NULL, NULL);
        }else if(strcmp(ch0->name, "MINUS")==0){
            Operand temp = createOperand(TEMP_OP, NULL, NULL);
            Operand zero = createOperand(CONS_OP, 0, NULL);
            Operand exp = Exp_IR(ch1);
            if(exp->isAddr == 1){
                Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
                insertInterCode(GET_CONTENT_IC, temp4content, exp, NULL, NULL);
                exp = temp4content;
            }
            if(exp->kind==CONS_OP){
                Operand optemp = createOperand(TEMP_OP, NULL, NULL);
                insertInterCode(ASSIGN_IC, optemp, exp, NULL, NULL);
                exp = optemp;
            }
            insertInterCode(SUB_IC, zero, exp, temp, NULL);
            op = temp;
        }
    }else if(ch3==NULL){
        if(strcmp(ch0->name, "LP")==0){
            // ( Exp )
            op = Exp_IR(ch1);
        }else if(strcmp(ch0->name, "ID")==0){
            // ID()
            FieldList field = lookUp(ch0->strval);
            if(field!=NULL){
                op = createOperand(TEMP_OP, NULL, NULL);
                if(strcmp(field->name, "read")==0){
                    insertInterCode(READ_IC, op, NULL, NULL, NULL);
                }else{
                    Operand funcOp = createOperand(FUNC_OP, NULL, ch0->strval);
                    insertInterCode(CALL_IC, op, funcOp, NULL, NULL);
                }
            }
        }else if(strcmp(ch1->name, "DOT")==0){
            // Exp DOT ID
            Operand exp = Exp_IR(ch0);
            // printf("Exp name: %s\n", exp->strval);
            Type t = exp->type;
            // if(exp->isAddr == 1){
            //     Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
            //     insertInterCode(GET_CONTENT_IC, temp4content, exp, NULL, NULL);
            //     exp = temp4content;
            //     // exp->isStruct = 1;
            // }
            // printf("here\n");
            if(exp->isStruct == 1){
                Operand tmp = createOperand(TEMP_OP, NULL, NULL);
                insertInterCode(GET_ADDR_IC, tmp, exp, NULL, NULL);
                tmp->strval = exp->strval;
                exp = tmp;
            }
            FieldList fields = NULL;
            op = createOperand(TEMP_OP, NULL, NULL);
            if(t==NULL){
                FieldList structField = lookUp(exp->strval);
                // printf("%s\n", ch0->strval);
                // temp = structVar + bias
                // if(structField==NULL) printf("here\n");
                fields = structField->type->u.structure;
            }else{
                fields = t->u.structure;
                // printf("%d\n", t->kind);
            }
            int bias = 0;
            while(fields!=NULL){
                if(fields->type->kind == TAG){
                    fields = fields->tail;
                    continue;
                }
                // printf("here\n");
                // printf("%s\n", fields->name);
                if(strcmp(fields->name, ch2->strval)==0){
                    break;
                }
                // printf("%s\n", fields->name);
                int off = 1;
                if(fields->type->kind==ARRAY){
                    off = fields->type->u.array.size;
                }
                bias += off*getTypeSize(fields->type);
                fields = fields->tail;
            }
            // printf("bias: %d\n", bias);
            if(bias!=0){
                Operand cons = createOperand(CONS_OP, bias, NULL);
                insertInterCode(ADD_IC, exp, cons, op, NULL);
            }else{
                op = exp;
            }
            // printf("here\n");
            op->isAddr = 1;
            op->strval = ch2->strval;
            // printf("here\n");
        }else {
            if(strcmp(ch1->name, "ASSIGNOP")==0){
                // Exp = Exp
                Operand op1 = Exp_IR(ch0);
                // if(op1->isAddr == 1){
                //     Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
                //     insertInterCode(GET_CONTENT_IC, temp4content, op1, NULL, NULL);
                //     op1 = temp4content;
                // }
                Operand op2 = Exp_IR(ch2);
                // printf("here\n");
                if(op1==NULL||op2==NULL)return NULL;
                if(op2->isAddr == 1 || op2->isArray){
                    Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
                    insertInterCode(GET_CONTENT_IC, temp4content, op2, NULL, NULL);
                    op2 = temp4content;
                }
                if(op1->isAddr == 1 || op1->isArray==1){
                    insertInterCode(WRITE_ADDR_IC, op1, op2, NULL, NULL);
                }
                else{
                    insertInterCode(ASSIGN_IC, op1, op2, NULL, NULL);
                    op = op1;
                }
            }else{
                if(strcmp(ch1->name, "AND")==0 || strcmp(ch1->name, "OR")==0 || strcmp(ch1->name, "RELOP")==0){
                    // | Exp AND Exp                   {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                    // | Exp OR Exp                    {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                    // | Exp RELOP Exp                 {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                    Operand label1 = createOperand(LABEL_OP, NULL, NULL);
                    Operand label2 = createOperand(LABEL_OP, NULL, NULL);
                    op = createOperand(TEMP_OP, NULL, NULL);
                    Operand zero = createOperand(CONS_OP, 0, NULL);
                    Operand one = createOperand(CONS_OP, 1, NULL);
                    insertInterCode(ASSIGN_IC, op, zero, NULL, NULL);
                    Condition_IR(node, label1, label2);
                    insertInterCode(LABEL_IC, label1, NULL, NULL, NULL);
                    // insertInterCode(DEC_IC, label1, zero, NULL, NULL);
                    insertInterCode(ASSIGN_IC, op, one, NULL, NULL);
                    insertInterCode(LABEL_IC, label2, NULL, NULL, NULL);
                }else{
                    // | Exp PLUS Exp                  {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                    // | Exp MINUS Exp                 {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                    // | Exp STAR Exp                  {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                    // | Exp DIV Exp                   {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}

                    // Operand temp1 = createOperand(TEMP_OP, NULL, NULL);
                    // Operand temp2 = createOperand(TEMP_OP, NULL, NULL);
                    Operand op1 = Exp_IR(ch0);
                    if(op1==NULL) return NULL;
                    if(op1->isAddr == 1 || op1->isArray == 1){
                        Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
                        insertInterCode(GET_CONTENT_IC, temp4content, op1, NULL, NULL);
                        op1 = temp4content;
                    }
                    Operand op2 = Exp_IR(ch2);
                    if(op2==NULL) return NULL;
                    if(op2->isAddr == 1 || op2->isArray == 1){
                        Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
                        insertInterCode(GET_CONTENT_IC, temp4content, op2, NULL, NULL);
                        op2 = temp4content;
                    }
                    if(op1!=NULL && op2!=NULL){
                        op = createOperand(TEMP_OP, NULL, NULL);
                        if(strcmp(ch1->name, "PLUS")==0){
                            insertInterCode(ADD_IC, op1, op2, op, NULL);
                        }else if(strcmp(ch1->name, "MINUS")==0){
                            // printf("here\n");
                            insertInterCode(SUB_IC, op1, op2, op, NULL);
                        }else if(strcmp(ch1->name, "STAR")==0){
                            insertInterCode(MUL_IC, op1, op2, op, NULL);
                        }else if(strcmp(ch1->name, "DIV")==0){
                            insertInterCode(DIV_IC, op1, op2, op, NULL);
                        }
                    }
                }

            }
        }
    }else{
        if(strcmp(ch0->name, "ID")==0){
            // ID LP Args RP
            FieldList field = lookUp(ch0->strval);
            if(field!=NULL){
                op = createOperand(TEMP_OP, NULL, NULL);
                if(strcmp(ch0->strval, "write")==0){
                    Operand intOp = Exp_IR(getChild(ch2, 0));
                    if(intOp==NULL) return NULL;
                    if(intOp->isAddr == 1){
                        Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
                        insertInterCode(GET_CONTENT_IC, temp4content, intOp, NULL, NULL);
                        intOp = temp4content;
                    }
                    if(intOp->kind==CONS_OP){
                        insertInterCode(ASSIGN_IC, op, intOp, NULL, NULL);
                        insertInterCode(WRITE_IC, op, NULL, NULL, NULL);
                        // op = intOp;
                    }else{
                        insertInterCode(WRITE_IC, intOp, NULL, NULL, NULL);
                    }
                }
                else{
                    Args_IR(ch2);
                    Operand funcOp = createOperand(FUNC_OP, NULL, ch0->strval);
                    insertInterCode(CALL_IC, op, funcOp, NULL, NULL);
                }
            }
        }else if(strcmp(ch0->name, "Exp")==0){
            // Exp LB Exp RB
            if(getChild(ch0, 3)!=NULL && strcmp(getChild(ch0, 3)->name, "RB")==0){
                highDimArrayError();
                return NULL;
            }
            Operand exp1 = Exp_IR(ch0);
            Operand exp2 = Exp_IR(ch2);
            // printf("here\n");
            if(exp1==NULL || exp2==NULL) return NULL;
            if(exp2->isAddr == 1){
                Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
                insertInterCode(GET_CONTENT_IC, temp4content, exp2, NULL, NULL);
                exp2 = temp4content;
            }
            if(exp1!=NULL && exp2!=NULL){
                // int elemSize = getTypeSize()
                // printf("%s\n", exp1->strval);
                FieldList field = lookUp(exp1->strval);
                if(field!=NULL){
                    // int offset = exp2->intval;
                    Type t = field->type;
                    if(t!=NULL){
                        int elemSize = getTypeSize(t);
                        Operand offset = createOperand(TEMP_OP, NULL, NULL);
                        Operand constOp = createOperand(CONS_OP, elemSize, NULL);
                        insertInterCode(MUL_IC, exp2, constOp, offset, NULL);
                        Operand elemAddr = createOperand(TEMP_OP, NULL, NULL);
                        // Operand tempOpAddr = createOperand(TEMP_OP, NULL, NULL);
                        // Operand arrayAddr = createOperand(TEMP_OP, NULL, NULL);
                        // Operand tempOp = createOperand(TEMP_OP, NULL, NULL);
                        if(exp1->isAddr == 1){
                            insertInterCode(ADD_IC, exp1, offset, elemAddr, NULL);
                        }else{
                            Operand arrayAddr = createOperand(TEMP_OP, NULL, NULL);
                            insertInterCode(GET_ADDR_IC, arrayAddr, exp1, NULL, NULL);
                            insertInterCode(ADD_IC, arrayAddr, offset, elemAddr, NULL);
                        }
                        // insertInterCode(GET_CONTENT_IC, tempOp, tempOpAddr, NULL, NULL);
                        op = elemAddr;
                        op->isAddr = 1;
                        // op->strval =
                    }
                    op->type = t->u.array.elem;
                    // printf("here\n");
                }
                // else{
                //     printf("here\n");
                // }
            }
        }
    }
    return op;
}

struct opList {
    Operand op;
    struct opList* next;
    struct opList* prev;
};

void Args_IR(NODE node)
{
    /*
        Args : Exp COMMA Args               {$$ = nonTerminalNode("Args", 3, $1, $2, $3);}
            | Exp                           {$$ = nonTerminalNode("Args", 1, $1);}
            ;
    */
    printDebug("Args_IR");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    Operand op = NULL;
    // int cnt = 0;
    struct opList* head = (struct opList*)malloc(sizeof(struct opList));
    struct opList* p = head;
    p->next = NULL;
    p->prev = NULL;

    while(ch0!=NULL){
        op = Exp_IR(ch0);
        if(op==NULL) return;
        if(op->isAddr == 1){
            Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
            insertInterCode(GET_CONTENT_IC, temp4content, op, NULL, NULL);
            op = temp4content;
        }
        if(op->isStruct==1){
            Operand temp = createOperand(TEMP_OP, NULL, NULL);
            insertInterCode(GET_ADDR_IC, temp, op, NULL, NULL);
            temp->strval = op->strval;
            op = temp;
        }else{
            if(op->kind == CONS_OP){
                Operand temp = createOperand(TEMP_OP, NULL, NULL);
                insertInterCode(ASSIGN_IC, temp, op, NULL, NULL);
                op = temp;
            }
        }
        // insertInterCode(ARG_IC, op, NULL, NULL, NULL);
        // cnt++;
        p->next = (struct opList*)malloc(sizeof(struct opList));
        p->next->next = NULL;
        p->next->prev = p;
        p->next->op = op;
        p = p->next;
        
        if(ch2!=NULL){
            ch0 = getChild(ch2, 0);
            ch2 = getChild(ch2, 2);
            // if(strcmp(ch0->name, "Exp")!=0)break;
        }else{
            ch0 = ch2;
        }
        // printf("here: %d\n", op->intval);
        // if(cnt==5)break;
    }
    while(p->prev!=NULL){
        insertInterCode(ARG_IC, p->op, NULL, NULL, NULL);
        p = p->prev;
    }
}

static char* getReverseRELOP(char* relop)
{
    if(strcmp(relop, "==")==0){
        return "!=";
    }
    if(strcmp(relop, "!=")==0){
        return "==";
    }
    if(strcmp(relop, ">=")==0){
        return "<";
    }
    if(strcmp(relop, "<=")==0){
        return ">";
    }
    if(strcmp(relop, ">")==0){
        return "<=";
    }
    if(strcmp(relop, "<")==0){
        return ">=";
    }
}

void Condition_IR(NODE node, Operand labelTrue, Operand labelFalse)
{
    /*
    Exp : Exp ASSIGNOP Exp              {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp AND Exp                   {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp OR Exp                    {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp RELOP Exp                 {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp PLUS Exp                  {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp MINUS Exp                 {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp STAR Exp                  {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | Exp DIV Exp                   {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | LP Exp RP                     {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | MINUS Exp                     {$$ = nonTerminalNode("Exp", 2, $1, $2);}
        | NOT Exp                       {$$ = nonTerminalNode("Exp", 2, $1, $2);}
        | Exp DOT ID                    {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
        | ID                            {$$ = nonTerminalNode("Exp", 1, $1);}
        | INT                           {$$ = nonTerminalNode("Exp", 1, $1);}
        | FLOAT                         {$$ = nonTerminalNode("Exp", 1, $1);}
        ;
    */
    printDebug("Condition_IR");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    // if(ch0!=NULL) printf("%s\n", ch0->name);
    NODE ch1 = getChild(node, 1);
    // if(ch1!=NULL) printf("%s\n", ch1->name);
    NODE ch2 = getChild(node, 2);
    // if(ch2!=NULL) printf("%s\n", ch2->name);
    Operand zero = createOperand(CONS_OP, 0, NULL);
    if(ch0!=NULL && strcmp(ch0->name, "NOT")==0){
        Condition_IR(ch1, labelFalse, labelTrue);
    }else if(ch1!=NULL && (strcmp(ch1->name, "AND")==0)){
        Operand label = createOperand(LABEL_OP, NULL, NULL);
        Condition_IR(ch0, label, labelFalse);
        insertInterCode(LABEL_IC, label, NULL, NULL, NULL);
        Condition_IR(ch2, labelTrue, labelFalse);
    }else if(ch1!=NULL && strcmp(ch1->name, "OR")==0){
        Operand label = createOperand(LABEL_OP, NULL, NULL);
        Condition_IR(ch0, labelTrue, label);
        insertInterCode(LABEL_IC, label, NULL, NULL, NULL);
        Condition_IR(ch2, labelTrue, labelFalse);
    }else if(ch1!=NULL && strcmp(ch1->name, "RELOP")==0){
        Operand op1 = Exp_IR(ch0);
        Operand op2 = Exp_IR(ch2);
        if(op1==NULL || op2==NULL) return;
        if(op1->isAddr == 1){
            // printf("here!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            // printf("%d\n", op1->u.varNo);
            Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
            insertInterCode(GET_CONTENT_IC, temp4content, op1, NULL, NULL);
            op1 = temp4content;
        }
        if(op2->isAddr == 1){
            Operand temp4content = createOperand(TEMP_OP, NULL, NULL);
            insertInterCode(GET_CONTENT_IC, temp4content, op2, NULL, NULL);
            op2 = temp4content;
        }
        char* relop = ch1->strval;
        if(op1->kind == CONS_OP){
            Operand op1temp = createOperand(TEMP_OP, NULL, NULL);
            insertInterCode(ASSIGN_IC, op1temp, op1, NULL, NULL);
            op1 = op1temp;
        }
        if(op2->kind == CONS_OP){
            Operand op2temp = createOperand(TEMP_OP, NULL, NULL);
            insertInterCode(ASSIGN_IC, op2temp, op2, NULL, NULL);
            op2 = op2temp;
        }
        if(labelTrue!=NULL && labelFalse!=NULL){
            insertInterCode(IF_GOTO_IC, op1, op2, labelTrue, relop);
            insertInterCode(GOTO_IC, labelFalse, NULL, NULL, NULL);
        }else if(labelTrue!=NULL){
            insertInterCode(IF_GOTO_IC, op1, op2, labelTrue, relop);
        }else if(labelFalse!=NULL){
            insertInterCode(IF_GOTO_IC, op1, op2, labelFalse, getReverseRELOP(relop));
        }
    }else{
        Operand op = Exp_IR(node);
        if(op==NULL) return;
        if(labelTrue!=NULL && labelFalse!=NULL){
            insertInterCode(IF_GOTO_IC, op, zero, labelTrue, "!=");
            insertInterCode(GOTO_IC, labelFalse, NULL, NULL, NULL);
        }else if(labelTrue!=NULL){
            insertInterCode(IF_GOTO_IC, op, zero, labelTrue, "!=");
        }else if(labelFalse!=NULL){
            insertInterCode(IF_GOTO_IC, op, zero, labelFalse, "==");
        }
    }
}