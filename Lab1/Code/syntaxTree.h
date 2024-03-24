#ifndef _SYNTAX_TREE_
#define _SYNTAX_TREE_

#include <stdio.h>
#include <stdarg.h> // variable argument lib
#include <stdlib.h>

#define true 1
#define false 0

enum TYPES {INT_, FLOAT_, ID_, TYPE_, OTHER_};

// only isToken == true can a TreeNode have type field
struct TreeNode{
    int row;
    int isToken;
    char name[100];
    enum TYPES type;
    // vals: int/float/str
    int intval;
    float floatval;
    char strval[100];

    struct TreeNode* child;
    struct TreeNode* sibling;
};

struct TreeNode* getNode(char* name, char* val, enum TYPES type);
struct TreeNode* nonTerminalNode(char* name, int num, ...);
void printTree(struct TreeNode* root, int depth);

#endif