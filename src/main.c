#include <stdio.h>
#include <string.h>

#include <ast/ast.h>
#include <parser/grammar.tab.h>
#include <symtab/symtab.h>
#include <backend/basicblocks.h>

int yylex();
void yyerror(const char *s);

extern int yydebug;

extern symbol_scope *file;
extern symbol_scope *current;

// src/lexer/lexer.lex
extern FILE *yyin;

extern char yyin_name[4096];
extern int line_num;

extern struct bb_cursor cursor;

int main(int argc, char *argv[])
{
    // set up initial scope
    file = symtab_create(
            NULL,
            SCOPE_GLOBAL,
            yyin_name,
            1
            );
    current = file;

    cursor.fn_num_counter = 1;
    cursor.bb_num_counter = 1;
    cursor.reg_count = 1;

    if( argc > 1 )
    {
        strcpy(yyin_name, argv[1]);
        yyin = fopen(argv[1], "r");
    }

    yydebug = 0;

    yyparse();

    cursorprint();
}
