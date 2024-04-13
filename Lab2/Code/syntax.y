%locations

%{
#include <stdio.h>
#include "lex.yy.c"
#include "syntaxTree.h"

extern int yylineno;
extern struct TreeNode* root;
extern syntax_error_cnt;
extern int flag_A;
%}

%union{
    struct TreeNode* node;
}

// High-level Definitions
%type <node> Program
%type <node> ExtDefList
%type <node> ExtDef
%type <node> ExtDecList

// Specifiers
%type <node> Specifier
%type <node> StructSpecifier
%type <node> OptTag
%type <node> Tag

// Declarators
%type <node> VarDec
%type <node> FunDec
%type <node> VarList
%type <node> ParamDec

// Statements
%type <node> CompSt
%type <node> StmtList
%type <node> Stmt

// Local Definitions
%type <node> DefList
%type <node> Def
%type <node> DecList
%type <node> Dec

// Expressions
%type <node> Exp
%type <node> Args

// terminals from lexical.l
%token <node> INT FLOAT ID
%token <node> PLUS MINUS STAR DIV
%token <node> ASSIGNOP RELOP
%token <node> RETURN TYPE STRUCT
%token <node> LP LB LC RP RB RC
%token <node> IF ELSE WHILE
%token <node> COMMA SEMI DOT
%token <node> AND OR NOT

// 结合性和优先级
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LP RP LB RB 
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
// High-level Definitions
Program : ExtDefList                {$$=nonTerminalNode("Program", 1, $1); root=$$;}
    ;
ExtDefList : ExtDef ExtDefList      {$$=nonTerminalNode("ExtDefList", 2, $1, $2);}
    |                               {$$=NULL;epsilon("ExtDecList");}    
    ;
ExtDef : Specifier ExtDecList SEMI  {$$=nonTerminalNode("ExtDef", 3, $1, $2, $3);}
    | Specifier SEMI                {$$=nonTerminalNode("ExtDef", 2, $1, $2);}
    | Specifier FunDec CompSt       {$$=nonTerminalNode("ExtDef", 3, $1, $2, $3);}
    | Specifier error SEMI          {yyerrok;}
    ;
ExtDecList : VarDec                 {$$=nonTerminalNode("ExtDecList", 1, $1);}
    | VarDec COMMA ExtDecList       {$$=nonTerminalNode("ExtDecList", 3, $1, $2, $3);}
    ;
// Specifiers
Specifier : TYPE                    {$$=nonTerminalNode("Specifier", 1, $1);}
    | StructSpecifier               {$$=nonTerminalNode("Specifier", 1, $1);}
    ;
StructSpecifier : STRUCT OptTag LC DefList RC   {$$=nonTerminalNode("StructSpecifier", 5, $1, $2, $3, $4, $5);}
    | STRUCT Tag                    {$$=nonTerminalNode("StructSpecifier", 2, $1, $2);}
    ;
OptTag : ID                         {$$=nonTerminalNode("OptTag", 1, $1);}
    |                               {$$=NULL;epsilon("OptTag");}
    ;
Tag : ID                            {$$=nonTerminalNode("Tag", 1, $1);}
    ;
// Declarators
VarDec : ID                         {$$=nonTerminalNode("VarDec", 1, $1);}
    | VarDec LB INT RB              {$$=nonTerminalNode("VarDec", 4, $1, $2, $3, $4);}
    | VarDec LB error RB             {yyerrok;}
    ;
FunDec : ID LP VarList RP           {$$=nonTerminalNode("FunDec", 4, $1, $2, $3, $4);}
    | ID LP RP                      {$$=nonTerminalNode("FunDec", 3, $1, $2, $3);}
    | ID LP error RP                {yyerrok;}
    ;
VarList : ParamDec COMMA VarList    {$$=nonTerminalNode("VarList", 3, $1, $2, $3);}
    | ParamDec                      {$$=nonTerminalNode("VarList", 1, $1);}
    ;
ParamDec : Specifier VarDec         {$$=nonTerminalNode("ParamDec", 2, $1, $2);}
    ;
// Statements
CompSt : LC DefList StmtList RC     {$$=nonTerminalNode("CompSt", 4, $1, $2, $3, $4);}
    | LC error DefList StmtList RC  {yyerrok;}
    | LC DefList StmtList error StmtList RC {yyerrok;}
    ;
StmtList : Stmt StmtList            {$$=nonTerminalNode("StmtList", 2, $1, $2);}
    |                               {$$=NULL; epsilon("StmtList");}
    //| Stmt error                    {yyerrok;}
    ;
Stmt : Exp SEMI                     {$$=nonTerminalNode("Stmt", 2, $1, $2);}
    | CompSt                        {$$=nonTerminalNode("Stmt", 1, $1);}
    | RETURN Exp SEMI               {$$=nonTerminalNode("Stmt", 3, $1, $2, $3);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   {$$ = nonTerminalNode("Stmt", 5, $1, $2, $3, $4, $5);}
    | IF LP Exp RP Stmt ELSE Stmt   {$$ = nonTerminalNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
    | WHILE LP Exp RP Stmt          {$$ = nonTerminalNode("Stmt", 5, $1, $2, $3, $4, $5);}
    //| error SEMI                    {yyerrok;}
    | WHILE error RC                {yyerrok;}
    | WHILE LP Exp RP error SEMI              {yyerrok;}
    //| Exp error                     {yyerrok;printf("here**********************\n");}
    ;
// Local Definitions
DefList : Def DefList               {$$ = nonTerminalNode("DefList", 2, $1, $2);}
    |                               {$$=NULL;epsilon("DefList");}
    | error SEMI                    {yyerrok;}
    ;
Def : Specifier DecList SEMI        {$$ = nonTerminalNode("Def", 3, $1, $2, $3);}
    | Specifier error SEMI          {yyerrok;}
    ;
DecList : Dec                       {$$ = nonTerminalNode("DecList", 1, $1);}
    | Dec COMMA DecList             {$$ = nonTerminalNode("DecList", 3, $1, $2, $3);}
    ;
Dec : VarDec                        {$$ = nonTerminalNode("Dec", 1, $1);}
    | VarDec ASSIGNOP Exp           {$$ = nonTerminalNode("Dec", 3, $1, $2, $3);}
    ;
// Expressions
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
    | Exp ASSIGNOP error            {yyerrok;}
    | Exp AND error                 {yyerrok;}
    | Exp OR error                  {yyerrok;}
    | Exp RELOP error               {yyerrok;}
    | Exp PLUS error                {yyerrok;}
    | Exp MINUS error               {yyerrok;}
    | Exp STAR error                {yyerrok;}
    | Exp DIV error                 {yyerrok;}
    | MINUS error                   {yyerrok;}
    | NOT error                     {yyerrok;}
    | LP error RP                   {yyerrok;}
    | ID LP error RP                {yyerrok;}
    | ID LP error SEMI              {yyerrok;}
    | Exp LB error RB               {yyerrok;}
    ;
Args : Exp COMMA Args               {$$ = nonTerminalNode("Args", 3, $1, $2, $3);}
    | Exp                           {$$ = nonTerminalNode("Args", 1, $1);}
    ;

%%
yyerror(char* msg) {
    //fprintf(stderr, "error: %s\n", msg);
    syntax_error_cnt++;
    if(flag_A==0){
        printf("Error type B at Line %d: %s\n", yylineno, msg);
    }
}

void epsilon(char* name)
{
    //printf("Epsilon Reduction: %s!\n", name);
}