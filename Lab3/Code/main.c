#include <stdio.h>
#include "syntaxTree.h"
#include "semanticAnalysis.h"
#include "interRepres.h"

extern FILE* yyin;
extern int yylineno;
struct TreeNode* root = NULL;
int syntax_error_cnt;
int lexical_error_cnt;

int main(int argc, char** argv)
{
    if(argc <= 1) return 1;
    FILE* f1 = fopen(argv[1], "r");
    if(!f1){
        perror(argv[1]);
        return 1;
    }
    FILE* f2 = fopen(argv[2], "wt+");
    if(!f2){
        perror(argv[2]);
        return 1;
    }

    yyrestart(f1);
    yyparse();

    if(lexical_error_cnt==0 && syntax_error_cnt==0){
        // printTree(root, 0);
        // printf("Starting symantic analysis!\n");
        symanticAnalysis(root);
        // printf("END of symantic analysis!\n");
        generateIR(root, f2);
    }
    fclose(f1);
    fclose(f2);
    return 0;
}