#ifndef _OBJ_CODE_
#define _OBJ_CODE_

// initialize code for all object codes
#define INITCODE ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n" \
                "read:\n\tli $v0, 4\n\tla $a0, _prompt\n\tsyscall\n\tli $v0, 5\n\tsyscall\n\tjr $ra\n\n"     \
                "write:\n\tli $v0, 1\n\tsyscall\n\tli $v0, 4\n\tla $a0, _ret\n\tsyscall\n\tmove $v0, $0\n\tjr $ra\n\n"

#include "interRepres.h"

extern InterCodes interCodeHead;

typedef struct VarDesc_* VarDesc;

struct Register {
    char* name;
    int valid;
    VarDesc varDesc; // where in memory to save the data
} ;

struct VarDesc_ {
    char name[50];
    int regID;
    int offset;
    VarDesc next;
} ;

VarDesc getVarDesc(Operand op);
void createVarDesc(Operand op);

// register operations
void initRegisters();
int getReg(Operand op, FILE* file);
void saveRegValue(int idx, FILE* file);

// enter function
void generateObjectCode(FILE* file);

// sub-functions for all object codes
void genLabelCode(InterCodes temp, FILE* file);
void genFunctionCode(InterCodes temp, FILE* file);
void genAssignCode(InterCodes temp, FILE* file);
void genAddCode(InterCodes temp, FILE* file);
void genSubCode(InterCodes temp, FILE* file);
void genMulCode(InterCodes temp, FILE* file);
void genDivCode(InterCodes temp, FILE* file);
void genGetAddrCode(InterCodes temp, FILE* file);
void genGetContentCode(InterCodes temp, FILE* file);
void genWriteAddrCode(InterCodes temp, FILE* file);
void genGotoCode(InterCodes temp, FILE* file);
void genIfGotoCode(InterCodes temp, FILE* file);
void genReturnCode(InterCodes temp, FILE* file);
void genDecCode(InterCodes temp, FILE* file);
void genArgCode(InterCodes temp, FILE* file);
void genCallCode(InterCodes temp, FILE* file);
void genParamCode(InterCodes temp, FILE* file);
void genReadCode(InterCodes temp, FILE* file);
void genWriteCode(InterCodes temp, FILE* file);



#endif

