#include <stdio.h>

#include <parser/grammar.tab.h>
#include <symtab/symtab.h>

int yylex();
void yyerror(const char *s);

extern int yydebug;

extern symbol_scope *file;
extern symbol_scope *current;

// src/lexer/lexer.lex
extern FILE *yyin;

extern char yyin_name[4096];
extern int line_num;

int main(int argc, char *argv[])
{
    file = symtab_create(
            NULL,
            SCOPE_GLOBAL,
            yyin_name,
            1
            );

    current = file;

    if( argc > 1 )
    {
        yyin = fopen(argv[1], "r");
    }

    yydebug = 0;

    yyparse();
}
