#ifndef _INTER_REPRES_
#define _INTER_REPRES_

#include <stdio.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "syntaxTree.h"
#define NODE struct TreeNode*
#define MAXOPS 500
typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct InterCodes_* InterCodes;

struct Operand_ {
    enum {
        VAR_OP,
        CONS_OP,
        TEMP_OP,
        FUNC_OP,
        LABEL_OP
    } kind;
    union {
        int varNo;
        int tempNo;
        int labelNo;
    } u;
    char* strval;
    int intval;
    float floatval;
    int size;
    int isAddr;
    int isStruct;
} ;


struct InterCode_ {
    enum {
        LABEL_IC,       // =0
        FUNCTION_IC,
        ASSIGN_IC,
        ADD_IC,
        SUB_IC,
        MUL_IC,         // =5
        DIV_IC,
        GET_ADDR_IC,
        GET_CONTENT_IC,
        WRITE_ADDR_IC,
        GOTO_IC,        // =10
        IF_GOTO_IC,
        RETURN_IC,
        DEC_IC,
        ARG_IC,
        CALL_IC,        // =15
        PARAM_IC,
        READ_IC,
        WRITE_IC        // =18
    } kind;
    union{
        struct {
            Operand op;
        } singleOP; // LABEL FUNCTION GOTO RETURN ARG PARAM READ WRITE

        struct {
            Operand op1, op2;
        } binOP; // ASSIGN GET_ADDR GET_CONTENT WRITE_ADDR DEC CALL

        struct {
            Operand result, op1, op2;
        } triOP; // ADD SUB MUL DIV

        struct {
            Operand op1, op2, label;
            char* relop;
        } jumpOP; // IF_GOTO
    } u;
} ;

struct InterCodes_ {
    InterCode code;
    InterCodes prev, next;
};

Operand VAR_Operands[MAXOPS];

// IR operands
void insertInterCode(int kind, Operand op1, Operand op2, Operand result, char* operator);
Operand createOperand(int kind, int size, char* name); // to complete

// printer
void printInterCode(FILE* file);

// starter
void generateIR(NODE root, FILE* file);

// Inter Code Generation
void Program_IR(NODE node);
void ExtDefList_IR(NODE node);
void ExtDef_IR(NODE node);
// void Specifier_IR(NODE node);
// void StructSpecifier_IR(NODE node);
// void OptTag_IR(NODE node);
void DefList_IR(NODE node);
void Def_IR(NODE node);
void DecList_IR(NODE node);
void Dec_IR(NODE node);
// void Tag_IR(NODE node);
void ExtDecList_IR(NODE node);
Operand VarDec_IR(NODE node, int param);
void FunDec_IR(NODE node);
void VarList_IR(NODE node);
void ParamDec_IR(NODE node);
void CompSt_IR(NODE node);
void StmtList_IR(NODE node);
void Stmt_IR(NODE node);
Operand Exp_IR(NODE node);
void Args_IR(NODE node);
void Condition_IR(NODE node, Operand labelTrue, Operand labelFalse);

#endif