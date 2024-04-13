#ifndef _SEMANTIC_ANALYSIS_
#define _SEMANTIC_ANALYSIS_

#include "syntaxTree.h"
#include "symbolTable.h"

#define NODE struct TreeNode*

void printError(int errID, int errLine, char* errMsg);
struct TreeNode* getChild(struct TreeNode* node, int n);
void symanticAnalysis(struct TreeNode* root);
void Program_check(NODE node);
void ExtDefList_check(NODE node);
void ExtDef_check(NODE node);
Type Specifier_check(NODE node);
Type StructSpecifier_check(NODE node);
char* OptTag_check(NODE node);
void DefList_check(NODE node, FieldList field);
void Def_check(NODE node, FieldList field);
void DecList_check(NODE node, Type type, FieldList field);
void Dec_check(NODE node, Type type, FieldList field);
char* Tag_check(NODE node);
void ExtDecList_check(NODE node, Type type);
FieldList VarDec_check(NODE node, Type type, FieldList field);
void FunDec_check(NODE node, Type type);
void VarList_check(NODE node, FieldList field);
void ParamDec_check(NODE node, FieldList field);
void CompSt_check(NODE node, Type type);
void StmtList_check(NODE node, Type type);
void Stmt_check(NODE node, Type type);
Type Exp_check(NODE node);
FieldList Args_check(NODE node);
bool sameType(Type t1, Type t2);
bool argMatch(FieldList f1, FieldList f2);
#endif