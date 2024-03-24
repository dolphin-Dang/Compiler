%{
#include "syntax.tab.h"
#include "syntaxTree.h"

int yycolumn = 1;
extern int lexical_error_cnt;
void addNode(char* name, char* val, enum TYPES type);

int flag_A = 0;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}

%option yylineno

COMMENT "//"
COMMENTL "/*"
SPACE [ \t]
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE int|float
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while

digit [0-9]
letter [a-zA-Z_]
INT 0|[1-9]{digit}*
FLOAT {digit}+[.]{digit}+
ID {letter}({letter}|{digit})*
WRONGID {digit}{ID}
WRONGFLOAT {DOT}{digit}+


%%
\n {yycolumn=1;flag_A=0;}
{SPACE} {}
{SEMI} {addNode("SEMI", yytext, OTHER_); return SEMI;}
{COMMA} {addNode("COMMA", yytext, OTHER_); return COMMA;}
{ASSIGNOP} {addNode("ASSIGNOP", yytext, OTHER_); return ASSIGNOP;}
{RELOP} {addNode("RELOP", yytext, OTHER_); return RELOP;}
{PLUS} {addNode("PLUS", yytext, OTHER_); return PLUS;}
{MINUS} {addNode("MINUS", yytext, OTHER_); return MINUS;}
{STAR} {addNode("STAR", yytext, OTHER_); return STAR;}
{DIV} {addNode("DIV", yytext, OTHER_); return DIV;}
{AND} {addNode("AND", yytext, OTHER_); return AND;}
{OR} {addNode("OR", yytext, OTHER_); return OR;}
{DOT} {addNode("DOT", yytext, OTHER_); return DOT;}
{NOT} {addNode("NOT", yytext, OTHER_); return NOT;}
{TYPE} {addNode("TYPE", yytext, TYPE_); return TYPE;}
{LP} {addNode("LP", yytext, OTHER_); return LP;}
{RP} {addNode("RP", yytext, OTHER_); return RP;}
{LB} {addNode("LB", yytext, OTHER_); return LB;}
{RB} {addNode("RB", yytext, OTHER_); return RB;}
{LC} {addNode("LC", yytext, OTHER_); return LC;}
{RC} {addNode("RC", yytext, OTHER_); return RC;}
{STRUCT} {addNode("STRUCT", yytext, OTHER_); return STRUCT;}
{RETURN} {addNode("RETURN", yytext, OTHER_); return RETURN;}
{IF} {addNode("IF", yytext, OTHER_); return IF;}
{ELSE} {addNode("ELSE", yytext, OTHER_); return ELSE;}
{WHILE} {addNode("WHILE", yytext, OTHER_); return WHILE;}
{INT} {addNode("INT", yytext, INT_); return INT;}
{FLOAT} {addNode("FLOAT", yytext, FLOAT_); return FLOAT;}
{ID} {addNode("ID", yytext, ID_); return ID;}
{COMMENT} {char a; do{a=input(); }while(a!='\n');}
{COMMENTL} {handleLongComment();}

{WRONGID} {flag_A=1;printf("Error type A at Line %d: Wrong ID \'%s\'\n", yylineno, yytext);lexical_error_cnt++;}
{WRONGFLOAT} {flag_A=1;printf("Error type A at Line %d: Wrong FLOAT \'%s\'\n", yylineno, yytext);lexical_error_cnt++;}
. {flag_A=1;printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);lexical_error_cnt++;}
%%

void handleLongComment()
{
    char a, b;
    a=input();
    while(1){
        b=input();
        if(a=='*'&&b=='/'){
            break;
        }else{
            a=b;
        }
        if(a=='\0' || b=='\0'){
            flag_A = 1;
            printf("Error type A at Line %d: Endless comment!\n", yylineno);
            break;
        }
    }
}

void addNode(char* name, char* val, enum TYPES type)
{
    yylval.node = getNode(name, val, type);
    yylval.node->row = yylineno;
    //printf("%s: %s\n", name, val);
}