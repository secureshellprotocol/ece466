%debug
%define parse.error verbose
%define api.header.include {<parser/grammar.tab.h>}
%define api.value.type { struct yy_struct };

%{

    /* the bison zone */

#define YYDEBUG 1

#include <stdio.h>
#include <string.h>

#include <ast/ast.h>
#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>

// ease compiler complaints
int yylex();
void yyerror(const char *s);

// tree root
ast_node *root;

extern FILE *yyin;
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

%type primary_expression
%type postfix_expression
%type unary_expression
%type unary_operator
%type cast_expression
%type multiplicative_expression
%type additive_expression
%type shift_expression
%type relational_expression
%type equality_expression
%type and_expression
%type xor_expression
%type or_expression
%type logand_expression
%type logor_expression
%type conditional_expression
%type assignment_expression
%type assignment_operator
%type expression

%start terminal

%%

primary_expression:
                  IDENT {   /* lexer is performing a strcpy, 
                                so its uniquely storing the value */
                    $$.n = ast_create_ident($1);
                  }
                  | NUMBER  {
                    $$.n = ast_create_num($1);
                  }
                  | CHARLIT  {
                    $$.n = ast_create_charlit($1);
                  }
                  | STRING  {
                    $$.n = ast_create_string($1);
                  }
                  | '(' assignment_expression ')'  {
                    $$ = $2;
                  }
                  ;

postfix_expression:
                  primary_expression
                  | postfix_expression '[' assignment_expression ']'    {
                    ast_node *add = ast_create_binop((int)'+', $1.n, $3.n);
                    $$.n = ast_create_unaop((int)'*', add);
                  }
                  | postfix_expression '(' ')'  {
                    $$.n = ast_create_func($1.n, NULL);
                  }
                  | postfix_expression '(' argument_expression_list ')' {
                    $$.n = ast_create_func($1.n, $3.n);
                  }
                  | postfix_expression '.' IDENT    {
                    $$.n = ast_create_binop((int)'.', $1.n,
                        ast_create_ident($3));
                  }
                  | postfix_expression INDSEL IDENT {
                    ast_node *ptr_to_1 = ast_create_unaop((int)'*', $1.n);
                    $$.n = ast_create_binop((int)'.', ptr_to_1, 
                        ast_create_ident($3));
                  }
                  | postfix_expression PLUSPLUS {
                    $$.n = ast_create_unaop(PLUSPLUS, $1.n);
                  }
                  | postfix_expression MINUSMINUS   {
                    $$.n = ast_create_unaop(MINUSMINUS, $1.n);
                  }
                  ;

argument_expression_list:
                        assignment_expression
                        | argument_expression_list ',' assignment_expression
                        ;

unary_expression:
                postfix_expression
                | PLUSPLUS unary_expression {
                    ast_node *one = ast_create_constant(1);
                    $$.n = ast_create_binop((int)'+', $2.n, one);
                }
                | MINUSMINUS unary_expression   {
                    ast_node *one = ast_create_constant(1);
                    $$.n = ast_create_binop((int)'-', $2.n, one);
                }
                | unary_operator cast_expression    {
                    $$.n = ast_create_unaop($1.ulld, $2.n);
                }
                | SIZEOF unary_expression   { 
                    $$.n = ast_create_unaop(SIZEOF, $2.n);
                }
                ;

unary_operator:
              '&'   {$$.ulld = '&';}
              | '*' {$$.ulld = '*';}
              | '+' {$$.ulld = '+';}
              | '-' {$$.ulld = '-';}
              | '~' {$$.ulld = '~';}
              | '!' {$$.ulld = '!';}
              ;

/* temporary kludge __ we dont have typing *yet* */
cast_expression:
               unary_expression

multiplicative_expression:
                         cast_expression
                         | multiplicative_expression '*' cast_expression    {
                            $$.n = ast_create_binop((int)'*', $1.n, $3.n);
                         }
                         | multiplicative_expression '/' cast_expression    {
                            $$.n = ast_create_binop((int)'/', $1.n, $3.n);
                         }
 
                         | multiplicative_expression '%' cast_expression    {
                            $$.n = ast_create_binop((int)'%', $1.n, $3.n);
                         }
                         ;

additive_expression:
                   multiplicative_expression
                   | additive_expression '+' multiplicative_expression  {
                    $$.n = ast_create_binop((int)'+', $1.n, $3.n);
                   }
                   | additive_expression '-' multiplicative_expression  {
                    $$.n = ast_create_binop((int)'-', $1.n, $3.n);
                   }
                   ;

shift_expression:
                additive_expression
                | shift_expression SHL additive_expression  {
                    $$.n = ast_create_binop(SHL, $1.n, $3.n);
                }
                | shift_expression SHR additive_expression  {
                    $$.n = ast_create_binop(SHR, $1.n, $3.n);
                }
                ;

relational_expression:
                     shift_expression
                     | relational_expression '<' shift_expression   {
                        $$.n = ast_create_binop((int)'<', $1.n, $3.n);
                     }
                     | relational_expression '>' shift_expression   {
                        $$.n = ast_create_binop((int)'>', $1.n, $3.n);
                     }
 
                     | relational_expression LTEQ shift_expression   {
                        $$.n = ast_create_binop(LTEQ, $1.n, $3.n);
                     }
 
                     | relational_expression GTEQ shift_expression   {
                        $$.n = ast_create_binop(GTEQ, $1.n, $3.n);
                     }
                     ;

equality_expression:
                   relational_expression
                   | equality_expression EQEQ relational_expression {
                    $$.n = ast_create_binop(EQEQ, $1.n, $3.n);
                   }
                   | equality_expression NOTEQ relational_expression    {
                    $$.n = ast_create_binop(NOTEQ, $1.n, $3.n);
                   }
                   ;

and_expression:
              equality_expression
              | and_expression '&' equality_expression  {
                $$.n = ast_create_binop((int)'&', $1.n, $3.n);
              }
              ;

xor_expression:
              and_expression
              | xor_expression '^' and_expression   {
                $$.n = ast_create_binop((int)'^', $1.n, $3.n);
              }
              ;

or_expression:
             xor_expression
             | or_expression '|' xor_expression {
                $$.n = ast_create_binop((int)'|', $1.n, $3.n);
             }
             ;

logand_expression:
                 or_expression
                 | logand_expression LOGAND or_expression   {
                    $$.n = ast_create_binop(LOGAND, $1.n, $3.n);
                 }
                 ;

logor_expression:
                logand_expression
                | logor_expression LOGOR logand_expression  {
                    $$.n = ast_create_binop(LOGOR, $1.n, $3.n);
                }
                ;

conditional_expression:
                      logor_expression
                      | ternary_expression
                      ;

ternary_expression:
                      logor_expression '?' expression ':' conditional_expression {
                        $$.n = ast_create_ternop($1.n, $3.n, $5.n);
                      }
                      ;

assignment_expression: 
                     conditional_expression
                     | unary_expression assignment_operator
                     assignment_expression   {
                        $$.n = ast_create_binop($2.ulld, $1.n, $3.n);
                     }
                     ;

assignment_operator:
                   '='          {$$.ulld = '=';}
                   | TIMESEQ    {$$.ulld = TIMESEQ;}
                   | DIVEQ      {$$.ulld = DIVEQ;}
                   | MODEQ      {$$.ulld = MODEQ;}
                   | PLUSEQ     {$$.ulld = PLUSEQ;}
                   | MINUSEQ    {$$.ulld = MINUSEQ;}
                   | SHLEQ      {$$.ulld = SHLEQ;}
                   | SHREQ      {$$.ulld = SHREQ;}
                   | ANDEQ      {$$.ulld = ANDEQ;}
                   | XOREQ      {$$.ulld = XOREQ;}
                   | OREQ       {$$.ulld = OREQ;}
                   ;

expression:
          assignment_expression
          | expression ',' assignment_expression
          ;

terminal:
        expression ';'  {
            root = $$.n;
        }
        ;

%%

int main(int argc, char* argv[])
{
    yyin = fopen(argv[1], "r");
//    yydebug=1;

    yyparse();
    if(root!=NULL) astprint(root);
}

void yyerror(const char *s)
{
    fprintf(stderr, "\aYou have disturbed me almost to the point of \
insanity...There. I am insane now.\nparser: %s\n", s);
}
