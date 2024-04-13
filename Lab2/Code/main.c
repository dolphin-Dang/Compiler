#include <stdio.h>
#include "syntaxTree.h"
#include "semanticAnalysis.h"

extern FILE* yyin;
extern int yylineno;
struct TreeNode* root = NULL;
int syntax_error_cnt;
int lexical_error_cnt;

int main(int argc, char** argv)
{
    if(argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if(!f){
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if(lexical_error_cnt==0 && syntax_error_cnt==0){
        // printTree(root, 0);
        // printf("Starting symantic analysis!\n");
        symanticAnalysis(root);
        // printf("END of symantic analysis!\n");
    }
    return 0;
}