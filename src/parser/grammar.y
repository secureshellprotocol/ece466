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

%token IDENT
%token CHARLIT
%token STRING
%token NUMBER
%token INDSEL
%token PLUSPLUS
%token MINUSMINUS
%token SHL
%token SHR
%token LTEQ
%token GTEQ
%token EQEQ
%token NOTEQ
%token LOGAND
%token LOGOR
%token ELLIPSIS
%token TIMESEQ
%token DIVEQ
%token MODEQ
%token PLUSEQ
%token MINUSEQ
%token SHLEQ
%token SHREQ
%token ANDEQ
%token OREQ
%token XOREQ
%token AUTO
%token BREAK
%token CASE
%token CHAR
%token CONST
%token CONTINUE
%token DEFAULT
%token DO
%token DOUBLE
%token ELSE
%token ENUM
%token EXTERN
%token FLOAT
%token FOR
%token GOTO
%token IF
%token INLINE
%token INT
%token LONG
%token REGISTER
%token RESTRICT
%token RETURN
%token SHORT
%token SIGNED
%token SIZEOF
%token STATIC
%token STRUCT
%token SWITCH
%token TYPEDEF
%token UNION
%token UNSIGNED
%token VOID
%token VOLATILE
%token WHILE
%token BOOL
%token COMPLEX
%token IMAGINARY

%start program
%%

program: IDENT {
        printf("IDENT!!!\n");
       }

%%

int main(void)
{
    yyparse();
}

void yyerror(const char *s)
{
    fprintf(stderr, "parser: %s\n", s);
}

