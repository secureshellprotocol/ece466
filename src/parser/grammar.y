%debug
%define parse.error verbose
%define api.header.include {<parser/grammar.tab.h>}
%define api.value.type { struct yy_struct };

%{
#define YYDEBUG 1

#include <string.h>

#include <ast/ast.h>
#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>

// ease compiler complaints
int yylex();
void yyerror(const char *s);

// tree root
ast_node *root;
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

%start expression

%%

primary_expression:
                  IDENT {
                    $$.n = create_node(IDENT);
                    $$.n->ident.value=strdup($1.s);    /* leaky leaky */
                    /* wheres the length lebowski? */
                  }
                  | NUMBER  {
                    $$.n = create_node(NUMBER);
                    /* what, no floating point? */
                    $$.n->num.ival = $1.ulld;
                    $$.n->num.tags = $1.tags;
                  }
                  | STRING 
                  | CHARLIT
                  | '(' expression ')'
                  ;

postfix_expression:
                  primary_expression
                  | postfix_expression '[' expression ']'
                  | postfix_expression '.' IDENT
                  | postfix_expression INDSEL IDENT
                  | postfix_expression PLUSPLUS
                  | postfix_expression MINUSMINUS
                  ;


unary_expression:
                postfix_expression
                | PLUSPLUS unary_expression
                | MINUSMINUS unary_expression
                | unary_operator cast_expression
                ;

unary_operator:
              '&'
              | '*'
              | '+'
              | '_'
              | '~'
              | '!'
              ;

/* temporary kludge __ we dont have typing *yet* */
cast_expression:
               unary_expression

multiplicative_expression:
                         cast_expression
                         | multiplicative_expression '*' cast_expression
                         | multiplicative_expression '/' cast_expression
                         | multiplicative_expression '%' cast_expression
                         ;

additive_expression:
                   multiplicative_expression
                   | additive_expression '+' multiplicative_expression  {
                    $$.n = create_node(BINOP);
                    $$.n->binop.token=(int)'+';
                    
                    $$.n->binop.left=$1.n;
                    $$.n->binop.right=$3.n;
                   }
                   | additive_expression '-' multiplicative_expression
                   ;

shift_expression:
                additive_expression
                | shift_expression SHL additive_expression
                | shift_expression SHR additive_expression
                ;

relational_expression:
                     shift_expression
                     | relational_expression '<' shift_expression
                     | relational_expression '>' shift_expression
                     | relational_expression LTEQ shift_expression
                     | relational_expression GTEQ shift_expression
                     ;

equality_expression:
                   relational_expression
                   | equality_expression EQEQ relational_expression
                   | equality_expression NOTEQ relational_expression
                   ;

and_expression:
              equality_expression
              | and_expression '&' equality_expression
              ;

xor_expression:
              and_expression
              | xor_expression '^' and_expression
              ;

or_expression:
             xor_expression
             | or_expression '|' xor_expression
             ;

logand_expression:
                 or_expression
                 | logand_expression LOGAND or_expression
                 ;

logor_expression:
                logand_expression
                | logor_expression LOGOR logand_expression
                ;

conditional_expression:
                      logor_expression
                      | logor_expression '?' expression ':' conditional_expression
                      ;

assignment_expression: 
                     conditional_expression
                     | unary_expression assignment_operator
                     assignment_expression   {
                        $$.n=$2.n;
                        
                        $$.n->binop.left=$1.n;
                        $$.n->binop.right=$3.n;
                     }  /* youre being dumb
                        EVERYTHING SHOUDL GET A NODE CREATED AT A TERMINAL NODE
                        THEN
                        EVERYTHING SHOULD JUST BE ASSIGNMENTS FROM HEREIN.
                        THAT IS WAY SIMPLER */
                     ;

assignment_operator:
                   '=' {
                    $$.n = create_node(BINOP);
                    $$.n->binop.token=(int)'=';
                   }
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
                   ;

expression:
          assignment_expression ';' {
            root = $$.n;
          }
          ;


%%

int main(void)
{
    yyparse();
    if(root!=NULL) print_from_node(root);
}

void yyerror(const char *s)
{
    fprintf(stderr, "\aparser: %s\n", s);
}
