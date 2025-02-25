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



declaration:
           init_declarator_list

declaration_specifier:
                      storage_class_specifier
                      | storage_class_specifier declaration_specifier
                      | type_specifier 
                      | type_specifier declaration_specifier
                      | type_qualifier
                      | type_qualifier declaration_specifier
                      | function_specifier
                      | function_specifier declaration_specifier


init_declarator_list:
                    init_declarator
                    | init_declarator_list ',' init_declarator

init_declarator:
               declarator
               | declarator '=' initializer

storage_class_specifier:
                       TYPEDEF
                       | EXTERN
                       | STATIC
                       | AUTO
                       | REGISTER
type_specifier:
              VOID
              | CHAR
              | SHORT
              | INT
              | LONG
              | FLOAT
              | DOUBLE
              | SIGNED
              | UNSIGNED 
              | BOOL
              | COMPLEX
/*              | struct_or_union_specifier
              | enum_specifier
              | typedef_name */

type_qualifier:
              CONST
              | RESTRICT
              | VOLATILE

function_specifier:
                  INLINE

declarator:
          direct_declarator
          | pointer direct_declarator

direct_declarator:
                 IDENT
                 | '(' declarator ')'

pointer:
       '*' type_qualifier_list
       | '*' type_qualifier_list pointer
       | '*' pointer

type_qualifier_list:
                   type_qualifier
                   | type_qualifier_list type_qualifier

parameter_type_list:
                   parameter_list
                   | parameter_list ',' ELLIPSIS

parameter_list:
              parameter_declaration
              | parameter_list ',' parameter_declaration

parameter_declaration:
                     declaration_specifier declarator
                     | declaration_specifier abstract_declarator
                     | declaration_specifier

identifier_list:
               identifier
               identifier_list ',' identifier

initializer: 
           assignment-expression
           | '{' initializer_list '}'
           | '{' initializer_list '}'

initializer_list:
                designation initializer
                | initializer_list ',' designation initializer
                | initializer_list ',' initializer

designation:
           designator_list '='

designator_list:
               designator
               | designator_list designator

designator:
          '[' constant-expression ']'
          '.' IDENT

statement:
         labeled_statement
         | compound_statement
         | expression_statement
         | selection_statement
         | iteration_statement
         | jump_statement

labeled_statement:
                 IDENT ':' statement
                 | CASE constant_expression ':' statement
                 | DEFAULT ':' statement

expression_statement:
                    expression ';'

selection_statement:
                   IF '(' expression ')' statement
                   | IF '(' expression ')' statement ELSE statement
                   | switch '(' expression ')' statement

iteration_statement:
                   WHILE '(' expression ')' statement
                   | DO statement WHILE '(' expression ')' ';'
                   | for_statement

for_statement:
             FOR '(' for_arguments ')' statement

/*  icky permutation ahead 
    figure out precedence with this */
for_arguments:
             ';' ';'
             | expression ';' ';'
             | ';' expression ';'
             | ';' ';' expression
             | expression ';' expression ';'
             | ';' expression ';' expression
             | expression ';' expression ';' expression
             | declaration ';'
             | declaration expression ';'
             | declaration ';' expression
             | declaration expression ';' expression

jump_statement:
              GOTO IDENT ';'
              | CONTINUE ';'
              | BREAK ';'
              | RETURN ';'
              | RETURN expression ';'



%%

int main(void)
{
    yyparse();
}

void yyerror(const char *s)
{
    fprintf(stderr, "parser: %s\n", s);
}

