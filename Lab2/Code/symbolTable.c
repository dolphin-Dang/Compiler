#include "symbolTable.h"

unsigned int hash(char* name)
{
    int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~TABLE_SIZE) val = (val ^ (i >> 12)) & TABLE_SIZE;
    }
    return val;
}

void initTable()
{
    for(int i=0; i<TABLE_SIZE; i++){
        SymbolTable[i] = NULL;
        CrossTable[i] = NULL;
    }
}

void insertSymbol(FieldList field, int depth)
{
    // insert in the head
    unsigned int idx = hash(field->name);
    TableNode node = (TableNode)malloc(sizeof(struct TableNode_));
    node->field = field;
    node->next = SymbolTable[idx];
    node->crossNext = NULL;
    node->depth = depth;
    SymbolTable[idx] = node;
    // printf("Inserting field name: %s\n", field->name);
    // 添加作用域 选做 3.2
    if(field->type->kind == BASIC || field->type->kind == STRUCTURE || field->type->kind == ARRAY){
        // printf("Inserting type kind %d, field name %s\n", field->type->kind, field->name);
        if(CrossTable[depth]==NULL){
            CrossTable[depth] = node;
        }else{
            TableNode p = CrossTable[depth];
            while(p->crossNext!=NULL) p = p->crossNext;
            p->crossNext = node;
        }
    }
}

FieldList lookUp(char* name)
{
    unsigned int idx = hash(name);
    TableNode node = SymbolTable[idx];
    while(node!=NULL && node->field != NULL){
        if(strcmp(node->field->name, name)==0){
            return node->field;
        }
        node = node->next;
    }
    return NULL;
}

FieldList lookUpInScope(char* name, int depth)
{
    unsigned int idx = hash(name);
    TableNode node = CrossTable[depth];
    while(node!=NULL){
        if(strcmp(node->field->name, name)==0){
            return node->field;
        }
        node = node->crossNext;
    }
    return NULL;
}

FieldList lookUp4Usage(char* name, int depth)
{
    unsigned int idx = hash(name);
    for(int i=depth; i>=0; i--){
        TableNode node = CrossTable[i];
        while(node!=NULL){
            if(strcmp(node->field->name, name)==0){
                return node->field;
            }
            node = node->crossNext;
        }
    }
    return NULL;
}

void leaveScope(int depth)
{
    TableNode node = CrossTable[depth];
    while(node!=NULL){
        TableNode p = node->crossNext;
        node->crossNext = NULL;
        node = p;
    }
    CrossTable[depth] = NULL;
    // return;
}