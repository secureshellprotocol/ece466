%debug
%define parse.error verbose

%define api.header.include {<parser/grammar.tab.h>}

%define api.value.type { struct yy_struct }

%{
#define YYDEBUG 1
#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>
#include <parser/ast.h>

int yylex();
void yyerror(const char *s);

%}

%start program

%%

program: %empty

%%

int main(void)
{
    yyparse();
}

void yyerror(const char *s)
{
    fprintf(stderr, "Bad parse: %s\n", s);
}

