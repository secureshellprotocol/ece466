#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <james_utils.h>
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

static void segfault_announcer(int signal)
{
    STDERR("We're sorry; you have reached a number that has been disconnected or is no longer in service. If you feel you have reached this recording in error, please check the number and try your call again.");
    STDERR("segfaulted");
    abort();
}

struct sigaction sa = {};

int main(int argc, char *argv[])
{
    STDERR("Welcome to the Cringe C Compiler! Use gently!");
    sa.sa_handler = segfault_announcer;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGSEGV, &sa, NULL);

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
