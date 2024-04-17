#include "semanticAnalysis.h"

int depth=0;
static void printDebug(char* msg)
{
    // printf("In %s!\n", msg);
}

void printError(int errID, int errLine, char* errMsg)
{
    printf("Error type %d at Line %d: ", errID, errLine);
    switch(errID){
        case 1:{
            printf("Undefined variable %s!\n", errMsg);
            break;
        }
        case 2:{
            printf("Undefined function %s!\n", errMsg);
            break;
        }
        case 3:{
            printf("Re-defined variable %s!\n", errMsg);
            break;
        }
        case 4:{
            printf("Re-defined function %s!\n", errMsg);
            break;
        }
        case 5:{
            printf("Mismatched expression type!\n");
            break;
        }
        case 6:{
            printf("Right value at left side!\n");
            break;
        }
        case 7:{
            printf("Mismatched operand type!\n");
            break;
        }
        case 8:{
            printf("Mismatched return type with definition!\n");
            break;
        }
        case 9:{
            printf("Wrong arguments number!\n");
            break;
        }
        case 10:{
            printf("Not an array!\n");
            break;
        }
        case 11:{
            printf("Not a function!\n");
            break;
        }
        case 12:{
            printf("Only integer in array[]!\n");
            break;
        }
        case 13:{
            printf("Not a structure!\n");
            break;
        }
        case 14:{
            printf("Undefined field %s!\n", errMsg);
            break;
        }
        case 15:{
            printf("Wrong field definition %s!\n", errMsg);
            break;
        }
        case 16:{
            printf("Duplicated name %s!\n", errMsg);
            break;
        }
        case 17:{
            printf("Undefined structure %s!\n", errMsg);
            break;
        }
        default:{
            printf("Why R U calling me? Something went wrong!!!\n");
        }
    }
}

static struct TreeNode* getChild(struct TreeNode* node, int n)
{
    // printf("here");
    struct TreeNode* t = node->child;
    if(t==NULL){return t;}
    else{
        for(int i=1; i<=n; i++){
            t = t->sibling;
            if(t==NULL) return t;
        }
        return t;
    }
}

static void addFuncInit()
{
    // read
    FieldList readField = (FieldList)malloc(sizeof(struct FieldList_));
    readField->name = "read";
    readField->tail = NULL;
    readField->type = (Type)malloc(sizeof(struct Type_));
    readField->type->kind = FUNCTION;
    readField->type->u.function.argc = 0;
    readField->type->u.function.argv = NULL;
    Type ret1 = (Type)malloc(sizeof(struct Type_));
    ret1->kind = BASIC;
    ret1->u.basic = 0;
    readField->type->u.function.retType = ret1;
    insertSymbol(readField, depth);
    
    // write
    FieldList writeField = (FieldList)malloc(sizeof(struct FieldList_));
    writeField->name = "write";
    writeField->tail = NULL;
    writeField->type = (Type)malloc(sizeof(struct Type_));
    writeField->type->kind = FUNCTION;
    writeField->type->u.function.argc = 1;
    // arg int
    FieldList argField = (FieldList)malloc(sizeof(struct FieldList_));
    argField->name = NULL;
    argField->tail = NULL;
    argField->type = (Type)malloc(sizeof(struct Type_));
    argField->type->kind = BASIC;
    argField->type->u.basic = 0;
    writeField->type->u.function.argv = argField;
    // return int
    Type ret2 = (Type)malloc(sizeof(struct Type_));
    ret2->kind = BASIC;
    ret2->u.basic = 0;
    writeField->type->u.function.retType = ret2;
    insertSymbol(writeField, depth);
}

void symanticAnalysis(NODE node)
{
    printDebug("symanticAnalysis");
    initTable();
    addFuncInit();
    Program_check(node);
}

void Program_check(NODE node)
{
    /*
        Program : ExtDefList
            ;
    */
    printDebug("Program_check");
    if(node==NULL)return;
    NODE ch = getChild(node, 0);
    ExtDefList_check(ch);
}

void ExtDefList_check(NODE node)
{
    /*
        ExtDefList : ExtDef ExtDefList
            |                               
            ;
    */
    printDebug("ExtDefList_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    ExtDef_check(ch0);
    ExtDefList_check(ch1);
}

void ExtDef_check(NODE node)
{
    /*
        ExtDef : Specifier ExtDecList SEMI  {$$=nonTerminalNode("ExtDef", 3, $1, $2, $3);}
            | Specifier SEMI                {$$=nonTerminalNode("ExtDef", 2, $1, $2);}
            | Specifier FunDec CompSt       {$$=nonTerminalNode("ExtDef", 3, $1, $2, $3);}
            ;
    */
    printDebug("ExtDef_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    NODE ch2 = getChild(node, 2);
    Type type;
    if(ch0!=NULL){
        type = Specifier_check(ch0);
    }
    if(ch2!=NULL){
        // not Specifier SEMI
        if(ch1!=NULL && strcmp(ch1->name, "ExtDecList")==0){
            // Specifier ExtDecList SEMI
            ExtDecList_check(ch1, type);
        }else{
            // Specifier FunDec CompSt
            depth++;
            FunDec_check(ch1, type);
            CompSt_check(ch2, type);
            leaveScope(depth);
            depth--;
        }
    }
}

Type Specifier_check(NODE node)
{
    /*
        Specifier : TYPE                    {$$=nonTerminalNode("Specifier", 1, $1);}
            | StructSpecifier               {$$=nonTerminalNode("Specifier", 1, $1);}
            ;
    */
    printDebug("Specifier_check");
    if(node==NULL)return NULL;
    Type type = NULL;
    NODE ch0 = getChild(node, 0);
    // printf("here\n");
    // printf("name: %s\n", ch0->name);
    // if(ch0==NULL)printf("???\n");
    if(strcmp(ch0->name, "TYPE")==0){
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = BASIC;
        if(strcmp(ch0->strval, "int")==0){
            type->u.basic = 0;
        }else{
            type->u.basic = 1;
        }
    }else{
        // StructSpecifier
        type = StructSpecifier_check(ch0);
    }
    return type;
}

Type StructSpecifier_check(NODE node)
{
    /*
        StructSpecifier : STRUCT OptTag LC DefList RC   {$$=nonTerminalNode("StructSpecifier", 5, $1, $2, $3, $4, $5);}
            | STRUCT Tag                    {$$=nonTerminalNode("StructSpecifier", 2, $1, $2);}
            ;
    */
    printDebug("StructSpecifier_check");
    if(node==NULL)return NULL;
    NODE ch1 = getChild(node, 1);
    NODE ch2 = getChild(node, 2);
    NODE ch3 = getChild(node, 3);
    Type type = NULL;
    FieldList field = NULL;
    if(ch2==NULL){
        // STRUCT Tag
        char* tag = Tag_check(ch1);
        field = lookUp(tag);
        if(field==NULL){
            printError(17, ch1->row, tag);
        }
        // else{
        //     field = (FieldList)malloc(sizeof(struct FieldList_));
        //     field->name = tag;
        // }
    }else{
        // STRUCT OptTag LC DefList RC
        char* opttag = NULL;
        if(strcmp(ch1->name, "OptTag")==0){
            // have OptTag
            opttag = OptTag_check(ch1);
        }
        if(opttag!=NULL){
            // printf("here\n");
            field = lookUp(opttag);

            if(field!=NULL){
                printError(16, ch1->row, opttag);
            }
        }
        field = (FieldList)malloc(sizeof(struct FieldList_));
        field->name = opttag;
        field->tail = NULL;
        field->type = (Type)malloc(sizeof(struct Type_));
        field->type->kind = TAG;
        if(opttag!=NULL){
            // not anonymous struct
            // printf("Inserting tag: %s\n", field->name);
            insertSymbol(field, depth);
        }
        if(ch2!=NULL && (strcmp(ch2->name, "DefList")==0)){
            DefList_check(ch2, field);
            // printf("No tag structure\n");
        }else if(ch3!=NULL && (strcmp(ch3->name, "DefList")==0)){
            DefList_check(ch3, field);
            // printf("With tag structure\n");
        }
        // while(field!=NULL){
        //     printf("%s!!!\n", field->name);
        //     field = field->tail;
        // }
    }
    type = (Type)malloc(sizeof(struct Type_));
    type->kind = STRUCTURE;
    type->u.structure = field;
    return type;
}

char* OptTag_check(NODE node)
{
    printDebug("OptTag_check");
    if(node==NULL)return NULL;
    NODE ch0 = getChild(node, 0);
    return ch0->strval;
}

void DefList_check(NODE node, FieldList field)
{
    // field for its type to complete
    /*
        DefList : Def DefList               {$$ = nonTerminalNode("DefList", 2, $1, $2);}
            |                               {$$=NULL;epsilon("DefList");}
            ;
    */
    printDebug("DefList_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    Def_check(ch0, field);
    DefList_check(ch1, field);
}

void Def_check(NODE node, FieldList field)
{
    /*
        Def : Specifier DecList SEMI        {$$ = nonTerminalNode("Def", 3, $1, $2, $3);}
            ;
    */
    printDebug("Def_check");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    Type type = Specifier_check(ch0);
    DecList_check(ch1, type, field);
}

void DecList_check(NODE node, Type type, FieldList field)
{
    /*
        DecList : Dec                       {$$ = nonTerminalNode("DecList", 1, $1);}
            | Dec COMMA DecList             {$$ = nonTerminalNode("DecList", 3, $1, $2, $3);}
            ;
    */
    printDebug("DecList_check");
    if(node==NULL) return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    
    Dec_check(ch0, type, field);
    if(ch2!=NULL){
        DecList_check(ch2, type, field);
    }
}

void Dec_check(NODE node, Type type, FieldList field)
{
    /*
        Dec : VarDec                        {$$ = nonTerminalNode("Dec", 1, $1);}
            | VarDec ASSIGNOP Exp           {$$ = nonTerminalNode("Dec", 3, $1, $2, $3);}
            ;
    */
    printDebug("Dec_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    FieldList getField=NULL;
    if(ch2==NULL){
        // VarDec
        getField = VarDec_check(ch0, type, field);
        // put VarDec into structure field
        if(field!=NULL && field->type->kind==TAG){
            while(field->tail!=NULL){
                field = field->tail;
                if(strcmp(field->name, getField->name)==0){
                    // error 15.1: re-define in one structure
                    printError(15, ch0->row, getField->name);
                }
            }
            field->tail = getField;
            // if(getField==NULL)printf("getField==NULL\n");
            // else printf("%s\n", field->tail->name);
        }
    }else{
        // VarDec ASSIGNOP Exp
        getField = VarDec_check(ch0, type, field);
        Type t = Exp_check(ch2);
        // error 15.2: initialize in structure
        if(field!=NULL && field->type->kind==TAG){
            printError(15, ch0->row, getField->name);
            while(field->tail!=NULL){
                field = field->tail;
                if(strcmp(field->name, getField->name)==0){
                    // error 15.1: re-define in one structure
                    printError(15, ch0->row, getField->name);
                }
            }
            field->tail = getField;
        }else if(sameType(t, type)==0){
            printError(5, ch2->row, NULL);
        }
    }
}

char* Tag_check(NODE node)
{
    printDebug("Tag_check");
    if(node==NULL)return NULL;
    NODE ch0 = getChild(node, 0);
    return ch0->strval;
}

void ExtDecList_check(NODE node, Type type)
{
    /*
        ExtDecList : VarDec                 {$$=nonTerminalNode("ExtDecList", 1, $1);}
            | VarDec COMMA ExtDecList       {$$=nonTerminalNode("ExtDecList", 3, $1, $2, $3);}
            ;
    */
    printDebug("ExtDecList_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    VarDec_check(ch0, type, NULL);  // no field
    if(ch2!=NULL){
        ExtDecList_check(ch2, type);
    }
}

FieldList VarDec_check(NODE node, Type type, FieldList field)
{
    /*
        VarDec : ID                         {$$=nonTerminalNode("VarDec", 1, $1);}
            | VarDec LB INT RB              {$$=nonTerminalNode("VarDec", 4, $1, $2, $3, $4);}
            ;
    */
    printDebug("VarDec_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    FieldList thisfield=NULL;
    if(ch2==NULL){
        // ID
        char* id = ch0->strval;
        thisfield = (FieldList)malloc(sizeof(struct FieldList_));
        thisfield->name = id;
        // if(type==NULL){
        //     printf("%s has no type!\n", id);
        // }else{
        //     printf("%s has type: %d\n", id, type->kind);
        // }
        thisfield->type = type;
        thisfield->tail = NULL;
        if(field!=NULL && field->type->kind==TAG){
            // in a structure, no conflicts!
            // printf("Getting structure field: %s\n", id);
        }else{
            FieldList f = lookUpInScope(id, depth);
            FieldList ff = lookUp(id);
            if(f!=NULL || (ff!=NULL && ff->type->kind==TAG)){
                printError(3, ch0->row, ch0->strval);
            }else{
                insertSymbol(thisfield, depth);
                // printf("Insert : %s\n", thisfield->name);
            }
        }
    }else{
        // VarDec LB INT RB
        Type t = (Type)malloc(sizeof(struct Type_));
        t->kind = ARRAY;
        t->u.array.elem = type;
        t->u.array.size = ch2->intval;
        return VarDec_check(ch0, t, field);
    }
    return thisfield;
}

void FunDec_check(NODE node, Type type)
{
    /*
        FunDec : ID LP VarList RP           {$$=nonTerminalNode("FunDec", 4, $1, $2, $3, $4);}
            | ID LP RP                      {$$=nonTerminalNode("FunDec", 3, $1, $2, $3);}
            ;
    */
    printDebug("FunDec_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    NODE ch3 = getChild(node, 3);
    char* id = ch0->strval;
    FieldList f = lookUp(id);
    if(f!=NULL){
        printError(4, ch0->row, ch0->strval);
    }
    FieldList field = (FieldList)malloc(sizeof(struct FieldList_));
    field->name = id;
    field->tail = NULL;
    field->type = (Type)malloc(sizeof(struct Type_));
    field->type->kind = FUNCTION;
    field->type->u.function.argc = 0;
    field->type->u.function.argv = NULL;
    field->type->u.function.retType = type;
    insertSymbol(field, depth);
    if(ch3==NULL){
        // ID LP 
        // no need to change
    }else{
        // ID LP VarList RP
        VarList_check(ch2, field);
    }
    
}

void VarList_check(NODE node, FieldList field)
{
    /*
        VarList : ParamDec COMMA VarList    {$$=nonTerminalNode("VarList", 3, $1, $2, $3);}
            | ParamDec                      {$$=nonTerminalNode("VarList", 1, $1);}
            ;
    */
    printDebug("VarList_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    ParamDec_check(ch0, field);
    VarList_check(ch2, field);
}

void ParamDec_check(NODE node, FieldList field)
{
    /*
        ParamDec : Specifier VarDec         {$$=nonTerminalNode("ParamDec", 2, $1, $2);}
            ;
    */
    printDebug("ParamDec_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    Type type = Specifier_check(ch0);
    FieldList varField = VarDec_check(ch1, type, NULL);
    // insert this varField to function args list
    if(field!=NULL && field->type!=NULL && field->type->kind==FUNCTION && varField!=NULL && type!=NULL){
        field->type->u.function.argc++;
        FieldList t = field->type->u.function.argv;
        if(t==NULL){
            field->type->u.function.argv = varField;
        }else{
            while(t->tail!=NULL){
                t = t->tail;
            }
            t->tail = varField;
        }
    }
}

void CompSt_check(NODE node, Type type)
{
    /*
        CompSt : LC DefList StmtList RC     {$$=nonTerminalNode("CompSt", 4, $1, $2, $3, $4);}
            ;
    */
    printDebug("CompSt_check");
    if(node==NULL)return;
    NODE ch1 = getChild(node, 1);
    NODE ch2 = getChild(node, 2);
    // depth++;
    if(strcmp(ch1->name, "DefList")==0){
        DefList_check(ch1, NULL);
        StmtList_check(ch2, type);
    }else if(strcmp(ch1->name, "StmtList")==0){
        StmtList_check(ch1, type);
    }
    // leaveScope(depth);
    // depth--;
}

void StmtList_check(NODE node, Type type)
{
    /*
        StmtList : Stmt StmtList            {$$=nonTerminalNode("StmtList", 2, $1, $2);}
            |                               {$$=NULL; epsilon("StmtList");}
            ;
    */
    printDebug("StmtList_check");
    if(node==NULL)return;
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    // if(ch0==NULL)printf("ch0==NULL\n");
    // if(ch1==NULL)printf("ch1==NULL\n");
    Stmt_check(ch0, type);
    StmtList_check(ch1, type);
}

void Stmt_check(NODE node, Type type)
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
    printDebug("Stmt_check");
    if(node==NULL)return;
    // printf("here\n");
    NODE ch0 = getChild(node, 0);
    NODE ch1 = getChild(node, 1);
    NODE ch2 = getChild(node, 2);
    NODE ch3 = getChild(node, 3);
    NODE ch4 = getChild(node, 4);
    NODE ch6 = getChild(node, 6);
    if(ch1==NULL){
        // CompSt
        depth++;
        CompSt_check(ch0, type);
        leaveScope(depth);
        depth--;
    }else if(ch2==NULL){
        // Exp SEMI
        Exp_check(ch0);
    }else if(ch3==NULL){
        // RETURN Exp SEMI
        Type retType = Exp_check(ch1);
        // printf("here in RETURN\n");
        if(sameType(retType, type)==0){
            printError(8, ch2->row, NULL);
        }
    }else if(ch6!=NULL){
        // IF LP Exp RP Stmt ELSE Stmt
        Type t = Exp_check(ch2);
        if(t!=NULL && (t->kind!=BASIC || (t->kind==BASIC && t->u.basic!=0))){
            printError(7, ch2->row, NULL);
        }
        Stmt_check(ch4, type);
        Stmt_check(ch6, type);
    }else{
        if(strcmp(ch0->strval, "while")==0){
            // WHILE LP Exp RP Stmt
            Type t = Exp_check(ch2);
            if(t!=NULL && (t->kind!=BASIC || (t->kind==BASIC && t->u.basic!=0))){
                printError(7, ch2->row, NULL);
            }
            Stmt_check(ch4, type);
        }else{
            // IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
            Type t = Exp_check(ch2);
            if(t!=NULL && (t->kind!=BASIC || (t->kind==BASIC && t->u.basic!=0))){
                printError(7, ch2->row, NULL);
            }
            Stmt_check(ch4, type);
        }
    }
}

Type Exp_check(NODE node)
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
    printDebug("Exp_check");
    if(node==NULL)return NULL;
    // printf("%s\n", node->name);
    NODE ch0 = getChild(node, 0);
    // if(ch0!=NULL)printf("ch0: %s\n", ch0->strval);
    NODE ch1 = getChild(node, 1);
    // if(ch1!=NULL)printf("ch1: %s\n", ch1->strval);
    NODE ch2 = getChild(node, 2);
    // if(ch2!=NULL)printf("ch2: %s\n", ch2->strval);
    NODE ch3 = getChild(node, 3);
    // if(ch3!=NULL)printf("ch3: %s\n", ch3->strval);
    Type type=NULL;
    if(ch1==NULL){
        // ID INT FLOAT
        if(ch0->type==ID_){
            // printf("here\n");
            FieldList f = lookUp4Usage(ch0->strval, depth);
            // FieldList ff = lookUp(ch0->strval, depth);
            if(f==NULL){
                printError(1, node->row, ch0->strval);
                // printf("here\n");
            }else{
                type = f->type;
            }
        }else{
            type = (Type)malloc(sizeof(struct Type_));
            type->kind = BASIC;
            if(ch0->type==FLOAT_){
                type->u.basic = 1;
            }else if(ch0->type==INT_){
                type->u.basic = 0;
                // printf("here\n");
            }
        } 
    }else if(ch2==NULL){
        // MINUS Exp
        // NOT Exp
        type = Exp_check(ch1);
        if(strcmp(ch0->name, "NOT")==0){
            if(type->kind!=BASIC || (type->kind==BASIC && type->u.basic!=0)){
                printError(7, ch0->row, NULL);
            }
        }else if(strcmp(ch0->name, "MINUS")==0){
            if(type->kind!=BASIC){
                printError(7, ch0->row, NULL);
            }
        }
    }else if(ch3==NULL){
        if(strcmp(ch0->name, "LP")==0){
            // ( Exp )
            type = Exp_check(ch1);
        }else if(strcmp(ch0->name, "ID")==0){
            // ID()
            FieldList f = lookUp(ch0->strval);
            if(f==NULL){
                printError(2, ch0->row, ch0->strval);
            }else if(f->type->kind!=FUNCTION){
                printError(11, ch0->row, NULL);
            }else if(f->type->u.function.argc!=0){
                printError(9, ch0->row, NULL);
            }
            if(f!=NULL && f->type!=NULL){
                type = f->type->u.function.retType;
            }
        }else if(strcmp(ch1->name, "DOT")==0){
            // Exp DOT ID
            Type t = Exp_check(ch0);
            // printf("here in Exp DOT ID\n");
            if(t==NULL){
                // printf("t==NULL\n");
            }
            else if(t->kind!=STRUCTURE){
                printError(13, ch0->row, NULL);
            }else{
                // error 14
                FieldList tagField = t->u.structure;
                // while(tagField!=NULL){
                //     printf("%s~~~\n", tagField->name);
                //     tagField = tagField->tail;
                // }
                char* idName = ch2->strval;
                bool found = false;
                // printf("here\n");
                while(tagField!=NULL){
                    // printf("%s %s\n", tagField->name, idName);
                    if(tagField->name!=NULL && strcmp(tagField->name, idName)==0){
                        // printf("succ in pairing\n");
                        found = true;
                        type = tagField->type;
                        break;
                    }
                    tagField = tagField->tail;
                }
                if(!found){
                    printError(14, ch2->row, idName);
                }
            }
            
        }else {
            // TODO: all types of error at Exp calculation
            if(strcmp(ch1->name, "ASSIGNOP")==0){
                // Exp = Exp
                Type leftType = Exp_check(ch0);
                Type rightType = Exp_check(ch2);
                // printf("here\n");
                // if(leftType==NULL)printf("left null\n");
                // if(rightType==NULL)printf("right null\n");
                // printf("%d, %d\n", leftType->kind, rightType->kind);
                // printf("here\n");
                if(sameType(leftType, rightType)==0){
                    printError(5, ch0->row, NULL);
                }
                // printf("here\n");
                if(leftType!=NULL){
                    NODE ch0ch0 = getChild(ch0, 0);
                    NODE ch0ch1 = getChild(ch0, 1);
                    // ch0ch2 = getChild(ch0, 2);
                    NODE ch0ch3 = getChild(ch0, 3);
                    // printf("%s\n", ch0ch0->name);
                    // if(ch0ch1==NULL)printf("here\n");
                    // ID
                    bool flag1 = (ch0ch1==NULL && ch0ch0!=NULL && strcmp(ch0ch0->name, "ID")==0);
                    // Exp DOT ID
                    bool flag2 = (ch0ch1!=NULL && strcmp(ch0ch1->name, "DOT")==0);
                    // Exp LB Exp RB
                    bool flag3 = (ch0ch3!=NULL && strcmp(ch0ch0->name, "Exp")==0);
                    if(flag1 || flag2 || flag3){
                        // left side is left value

                    }else{
                        printError(6, ch0->row, NULL);
                    }
                    type = leftType;
                }
            }else{
                // | Exp AND Exp                   {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                // | Exp OR Exp                    {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                // | Exp RELOP Exp                 {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                // | Exp PLUS Exp                  {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                // | Exp MINUS Exp                 {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                // | Exp STAR Exp                  {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                // | Exp DIV Exp                   {$$ = nonTerminalNode("Exp", 3, $1, $2, $3);}
                // printf("here in Exp op Exp\n");
                // printf("op: %s\n", ch1->name);
                Type leftType = Exp_check(ch0);
                Type rightType = Exp_check(ch2);
                if(sameType(leftType, rightType)==0){
                    printError(7, ch0->row, NULL);
                }else if(leftType->kind!=BASIC){
                    printError(7, ch0->row, NULL);
                }
                // printf("here\n");
                type = leftType;
                if(strcmp(ch1->name, "RELOP")==0){
                    type = (Type)malloc(sizeof(struct Type_));
                    type->kind = 0;
                    type->u.basic = 0;
                }
            }

        }
    }else{
        if(strcmp(ch0->name, "ID")==0){
            // ID LP Args RP
            FieldList f = lookUp(ch0->strval);
            if(f==NULL){
                printError(2, ch0->row, ch0->strval);
            }else if(f->type->kind!=FUNCTION){
                printError(11, ch0->row, NULL);
            }else{
                FieldList argField = (FieldList)malloc(sizeof(struct FieldList_));
                argField->name=ch0->strval;
                argField->type = (Type)malloc(sizeof(struct Type_));
                argField->type->kind = FUNCTION;
                argField->type->u.function.argc = 0;
                argField->type->u.function.argv = Args_check(ch2);
                FieldList tmpField = argField->type->u.function.argv;
                while(tmpField!=NULL){
                    // printf("%d\n", tmpField->type->kind);
                    argField->type->u.function.argc++;
                    tmpField = tmpField->tail;
                }
                // printf("here\n");
                // if(argField->type->u.function.argv == NULL){
                //     printf("argField->type->u.function.argv is NULL\n");
                // }
                argField->type->u.function.retType = NULL;
                // if(f==NULL)printf("f==NULL\n");
                // if(argField==NULL)printf("argField==NULL\n");
                if(argMatch(f, argField)==0){
                    // printf("here\n");
                    printError(9, ch0->row, NULL);
                }
            }
            if(f!=NULL){
                type = f->type->u.function.retType;
            }
        }else if(strcmp(ch0->name, "Exp")==0){
            // Exp LB Exp RB
            Type t = Exp_check(ch0);
            if(t!=NULL && t->kind!=ARRAY){
                printError(10, ch0->row, NULL);
            }else if(t!=NULL){
                type = t->u.array.elem;
            }
            Type t2 = Exp_check(ch2);
            // printf("here\n");
            if(t2!=NULL && (t2->kind!=BASIC || t2->u.basic!=0)){
                printError(12, ch2->row, NULL);
            }
        }
    }
    // printf("here before Exp_check return\n");
    // if(type!=NULL)printf("%d\n", type->kind);
    return type;
}

FieldList Args_check(NODE node)
{
    /*
        Args : Exp COMMA Args               {$$ = nonTerminalNode("Args", 3, $1, $2, $3);}
            | Exp                           {$$ = nonTerminalNode("Args", 1, $1);}
            ;
    */
    printDebug("Args_check");
    if(node==NULL)return NULL;
    NODE ch0 = getChild(node, 0);
    NODE ch2 = getChild(node, 2);
    FieldList field = NULL;
    // int ret=0;
    if(ch0!=NULL){
        // printf("here in Args_check\n");
        field = (FieldList)malloc(sizeof(struct FieldList_));
        Type t = Exp_check(ch0);
        // printf("t->kind: %d\n", t->kind);
        if(t!=NULL){
            field->type = t;
            field->tail = NULL;
            field->name = ch0->strval;
            // printf("%d\n", field->type->kind);
        }else{
            printf("Something went wrong!\n");
            return NULL;
        }
        // ret++;
    }
    if(ch2!=NULL){
        // printf("More args\n");
        field->tail = Args_check(ch2);
    }
    // printf("here\n");
    return field;
}

bool sameType(Type t1, Type t2)
{
    printDebug("sameType");
    // printf("here in sameType\n");
    if(t1==NULL || t2==NULL) return false;
    if(t1->kind != t2->kind) return false;
    if(t1->kind == BASIC){
        return t1->u.basic == t2->u.basic;
    }else if(t1->kind == ARRAY){
        return (t1->u.array.size == t2->u.array.size) && sameType(t1->u.array.elem, t2->u.array.elem);
    }else if(t1->kind == STRUCTURE){
        // 名等价
        return strcmp(t1->u.structure->name, t2->u.structure->name)==0;
    }else{
        // no chance to be TAG or FUNCTION
        return false;
    }
}

bool argMatch(FieldList f1, FieldList f2)
{
    printDebug("argMatch");
    if(f1==NULL || f2==NULL) return false;
    if(f1->type->kind!=FUNCTION || f2->type->kind!=FUNCTION) return false;
    if(f1->type->u.function.argc!=f2->type->u.function.argc) return false;
    if(f1->type->u.function.argc==0) return true;
    FieldList t1 = f1->type->u.function.argv;
    FieldList t2 = f2->type->u.function.argv;
    // printf("argc: %d\n", f1->type->u.function.argc);
    while(t1!=NULL){
        // if(t1==NULL)printf("t1\n");
        // if(t2==NULL)printf("t2\n");
        // printf("here\n");
        if(sameType(t1->type, t2->type)==0){
            return false;
        }else{
            t1 = t1->tail;
            t2 = t2->tail;
        }
    }
    return true;
}