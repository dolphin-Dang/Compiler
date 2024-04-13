#ifndef _SYMBOL_TABLE_
#define _SYMBOL_TABLE_

#include <stdio.h>
#include <string.h>
#define true 1
#define false 0
#define bool _Bool
#define TABLE_SIZE 0x3fff
#define MAX_DEPTH 50

// enum bool {false, true};

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct TableNode_* TableNode;
// 类型表示
struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE, TAG, FUNCTION } kind;
    union
    {
        // 基本类型，0：int，1：float
        int basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct { Type elem; int size; } array;
        // 结构体类型信息是一个链表
        FieldList structure;
        // 函数参数
        struct {int argc; FieldList argv; Type retType;} function;
    } u;
};

struct FieldList_
{
    char* name; // 域的名字
    Type type; // 域的类型
    FieldList tail; // 下一个域
};

// 链表结构
struct TableNode_{
    TableNode next;
    TableNode crossNext;
    FieldList field;
    int depth;
};

TableNode SymbolTable[TABLE_SIZE];
TableNode CrossTable[MAX_DEPTH];

unsigned int hash(char* name);
void initTable();
void insertSymbol(FieldList field, int depth);
FieldList lookUp(char* name);
FieldList lookUpInScope(char* name, int depth); // for definition
FieldList lookUp4Usage(char* name, int depth); // for usage
void leaveScope(int depth);
#endif