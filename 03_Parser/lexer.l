%{
#include "parser.tab.h"
%}

%%
[0-9]+\.[0-9]+    { yylval.fval = atof(yytext); return FLOAT; }
[0-9]+            { yylval.ival = atoi(yytext); return NUMBER; }
LINE              { return LINE; }
CIRCLE            { return CIRCLE; }
ELLIPSE           { return ELLIPSE; }
":"               { return ':'; }
","               { return ','; }
"("               { return '('; }
")"               { return ')'; }
[ \t\r\n]+        { /* skip whitespace */ }
.                 { /* ignore other characters */ }
%%

int yywrap() { return 1; }