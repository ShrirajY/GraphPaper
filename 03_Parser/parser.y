%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);
extern FILE *yyin; // Declare yyin for Flex
%}

%union {
    float fval;
    int  ival;
}

%token LINE CIRCLE ELLIPSE
%token <fval> FLOAT
%token <ival> NUMBER

%type <fval> num

%%

input:
    /* empty */
  | input shape
  ;

num:
    FLOAT  { $$ = $1; }
  | NUMBER { $$ = (float)$1; }
  ;

shape:
    LINE ':' num ',' num ',' num ',' num ',' '(' NUMBER ',' NUMBER ',' NUMBER ')'
        { printf("Found LINE: start=(%.2f,%.2f) end=(%.2f,%.2f) color=(%d,%d,%d)\n", $3, $5, $7, $9, $12, $14, $16); }
  | CIRCLE ':' num ',' num ',' num ',' '(' NUMBER ',' NUMBER ',' NUMBER ')'
        { printf("Found CIRCLE: center=(%.2f,%.2f) radius=%.2f color=(%d,%d,%d)\n", $3, $5, $7, $10, $12, $14); }
  | ELLIPSE ':' num ',' num ',' num ',' num ',' '(' NUMBER ',' NUMBER ',' NUMBER ')'
        { printf("Found ELLIPSE: center=(%.2f,%.2f) a=%.2f b=%.2f color=(%d,%d,%d)\n", $3, $5, $7, $9, $12, $14, $16); }
;

%%

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }
        yyin = file;
    }
    return yyparse();
}

int yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}