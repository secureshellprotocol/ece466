%debug
%define parse.error verbose
%define api.header.include {<parser/grammar.tab.h>}
%define api.value.type { struct yy_struct }

%{
#define YYDEBUG 1

#include <ast/ast.h>
#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>


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

%%

primary_expression: 
                  IDENT
                  | NUMBER
                  | STRING
                  | CHARLIT
                  | '(' expression ')'

postfix_expression:
                  primary_expression
                  | postfix_expression '[' expression ']'
                  | postfix_expression '.' IDENT
                  | postfix_expression INDSEL IDENT
                  | postfix_expression PLUSPLUS
                  | postfix_expression MINUSMINUS

unary_expression:
                postfix_expression
                | PLUSPLUS unary_expression
                | MINUSMINUS unary_expression
                | unary_operator cast_expression

unary_operator:
              '&'
              | '*'
              | '+'
              | '_'
              | '~'
              | '!'

/* temporary kludge __ we dont have typing *yet* */
cast_expression:
               unary_expression

multiplicative_expression:
                         cast_expression
                         | multiplicative_expression '*' cast_expression
                         | multiplicative_expression '/' cast_expression
                         | multiplicative_expression '%' cast_expression

additive_expression:
                   multiplicative_expression
                   | additive_expression '+' multiplicative_expression
                   | additive_expression '_' multiplicative_expression

shift_expression:
                additive_expression
                | shift_expression SHL additive_expression
                | shift_expression SHR additive_expression

relational_expression:
                     shift_expression
                     | relational_expression '<' shift_expression
                     | relational_expression '>' shift_expression
                     | relational_expression LTEQ shift_expression
                     | relational_expression GTEQ shift_expression

equality_expression:
                   relational_expression
                   | equality_expression EQEQ relational_expression
                   | equality_expression NOTEQ relational_expression

and_expression:
              equality_expression
              | and_expression '&' equality_expression

xor_expression:
              and_expression
              | xor_expression '^' and_expression

or_expression:
             xor_expression
             | or_expression '|' xor_expression

logand_expression:
                 or_expression
                 | logand_expression LOGAND or_expression

logor_expression:
                logand_expression
                | logor_expression LOGOR logand_expression

conditional_expression:
                      logor_expression
                      | logor_expression '?' expression ':' conditional_expression

assignment_expression: 
                     conditional_expression
                     | unary_expression assignment_operator assignment_expression

assignment_operator:
                   '='
                   | TIMESEQ
                   | DIVEQ
                   | MODEQ
                   | PLUSEQ
                   | MINUSEQ
                   | SHLEQ
                   | SHREQ 
                   | ANDEQ
                   | XOREQ
                   | OREQ

expression:
          assignment_expression
          | expression ',' assignment_expression


%%

int main(void)
{
    yyparse();
}

void yyerror(const char *s)
{
    fprintf(stderr, "parser: %s\n", s);
}

