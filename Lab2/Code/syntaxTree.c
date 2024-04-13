#include "syntaxTree.h"

struct TreeNode* getNode(char* name, char* val, enum TYPES type)
{
    struct TreeNode* node = malloc(sizeof(struct TreeNode));
    node->row = -1;     // for terminals, don't need line number
    node->isToken = true;
    node->child = NULL;
    node->sibling = NULL;
    node->type = type;
    strcpy(node->name, name);
    strcpy(node->strval, val);
    if(type==INT_) node->intval=atoi(val);
    else if(type==FLOAT_) node->floatval=atof(val);
    // printf("In getNode(): %s\n", node->val);
    return node;
}

struct TreeNode* nonTerminalNode(char* name, int num, ...)
{
    // printf("---%s %d\n",name, num);
    // printf("Constructing tree, non-terminal: %s\n", name);
    int i=0; // loop counter for childs

    // variable argument analyze
    struct TreeNode* firstChild = malloc(sizeof(struct TreeNode));
    va_list valist;
    va_start(valist, num);
    // first child also can be NULL
    for(; i<num; i++){
        firstChild = (struct TreeNode*)va_arg(valist, struct TreeNode*);
        if(firstChild != NULL) break;
    }
    i++;
    // printf("First child name: %s\n", firstChild->name);
    // printf("First child val: %s\n", firstChild->strval);
    // init this father node
    struct TreeNode* node = malloc(sizeof(struct TreeNode));
    strcpy(node->name, name);
    node->row = firstChild->row;
    node->isToken = false;
    node->child = firstChild;
    node->sibling = NULL;

    // get other childs
    struct TreeNode* p = firstChild;
    for(; i<num; i++){
        // printf("%d\n",p);
        p->sibling = (struct TreeNode*)va_arg(valist, struct TreeNode*);
        if(p->sibling != NULL){
            // can be NULL for \epsilon production
            p = p->sibling;
        }
    }
    va_end(valist);
    // printf("End of constructing, non-terminal: %s\n", name);
    // free(firstChild);
    return node;
}

void printTree(struct TreeNode* node, int depth)
{
    if(node == NULL) return;
    for(int i=0; i<depth; i++){
        printf("  ");
    }
    if(!node->isToken){
        printf("%s (%d)\n", node->name, node->row);
        // printf(" *%d*", node);
    }else{
        printf("%s", node->name);
        // printf("**Node name: %s**", node->name);
        switch(node->type){
            case ID_:
            case TYPE_:
                printf(": %s\n", node->strval);
                break;
            case INT_:
                printf(": %d\n", node->intval);
                break;
            case FLOAT_:
                printf(": %.6f\n", node->floatval);
                break;
            default:
            printf("\n");
                break;
        }
    }
    printTree(node->child, depth+1);
    // printf("%d ", node->sibling);
    // printf("%d", node);
    // printf("here");
    printTree(node->sibling, depth);
}