%define parse.error verbose
%define api.header.include {<parser/grammar.tab.h>}
%define api.value.type { struct yy_struct };

%{

    /* the bison zone */

#define YYDEBUG 1

#include <stdio.h>
#include <string.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <ast/types.h>
#include <backend/basicblocks.h>
#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>
#include <symtab/symtab.h>

// ease compiler complaints
int yylex();
void yyerror(const char *s);

// file scope
symbol_scope *file;
symbol_scope *current;

// lex stats
extern FILE *yyin;

extern char yyin_name[4096];
extern int line_num;

// basic blocks
struct bb_cursor cursor = {0};

// ALERT -- API CHANGE
// ENTER SCOPE -- enter into an already existing table (ie, formed for a union
//  or struct)
//#define ENTER_SCOPE(symtab) \
//    current = symtab;

// CREATE SCOPE -- make a new scope using the SCOPE_ enums in 
//  include/symtab/symtab.h
#define CREATE_SCOPE(scope) \
    current = symtab_create(current, scope, yyin_name, line_num)

// current gets replaced with the previous scope.
#define EXIT_SCOPE() \
    current = symtab_destroy(current);

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

// shift ASAP on else
%right ')' ELSE

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

%start translation_unit

%%

primary_expression:
                  IDENT {   /* lexer is performing a strdup,
                        therefore value is in stable storage. */
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
                  | '(' expression ')'  {
                    $$.n = $2.n;
                  }
                  ;

postfix_expression:
                  primary_expression    { $$ = $1; }
                  | postfix_expression '[' assignment_expression ']'    {
                    /*ast_node *add = ast_create_binop('+', 
                        $1.n, 
                        ast_create_binop('*', $3.n, 
                            ast_create_unaop(SIZEOF, 
                                ast_create_unaop('*', $1.n)
                            )
                        )
                    );*/
                    ast_node *add = ast_create_binop('+', $1.n, $3.n);
                    $$.n = ast_create_unaop('*', add);
                  }
                  | postfix_expression '(' ')'  {
                    $$.n = ast_create_func_call($1.n, NULL);
                  }
                  | postfix_expression '(' argument_expression_list ')' {
                    $$.n = ast_create_func_call($1.n, $3.n);
                  }
                  | postfix_expression '.' IDENT    {
                    STDERR("Struct members not supported!");
                    $$.n = ast_create_binop('.', 
                        $1.n, ast_create_ident($3));
                  }
                  | postfix_expression INDSEL IDENT {
                    STDERR("Indirect selection (->) not supported!");
                    $$.n = ast_create_binop('.', 
                        ast_create_unaop('*', $1.n), 
                        ast_create_ident($3));
                  }
                  | postfix_expression PLUSPLUS {
                    $$.n = ast_create_unaop(PLUSPLUS, $1.n);
                  }
                  | postfix_expression MINUSMINUS   {
                    $$.n = ast_create_unaop(MINUSMINUS, $1.n);
                  }
/*                  | '(' type_name ')' '{' initializer_list '}' {
                    $$.n = ast_create_func(NULL, $2.n, $5.n);
                  }
                  | '(' type_name ')' '{' initializer_list ',' '}'  {
                    $$.n = ast_create_func(NULL, $2.n, $5.n);
                  }
*/                  ;

argument_expression_list:
                        assignment_expression   {
                            $$.n = ast_list_start($1.n);
                        }
                        | argument_expression_list ',' assignment_expression    {
                            $$.n = ast_list_insert($1.n, $3.n);
                        }
                        ;

unary_expression:
                postfix_expression  { $$ = $1; }
                | PLUSPLUS unary_expression {
                    ast_node *one = ast_create_constant(1);
                    $$.n = ast_create_binop('+', $2.n, one);
                }
                | MINUSMINUS unary_expression   {
                    ast_node *one = ast_create_constant(1);
                    $$.n = ast_create_binop('-', $2.n, one);
                }
                | unary_operator cast_expression    {
                    $$.n = ast_create_unaop($1.ulld, $2.n);
                }
                | SIZEOF unary_expression   { 
                    $$.n = ast_create_unaop(SIZEOF, $2.n);
                }
                | SIZEOF '(' type_name ')'  {
                    $$.n = ast_create_unaop(SIZEOF, $3.n);
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

cast_expression:
               unary_expression { $$ = $1; }
               | '(' type_name ')' cast_expression  {
                $$.n = ast_create_binop(TYPECAST, $2.n, $4.n);
               }
               ;

multiplicative_expression:
                         cast_expression    { $$ = $1; }
                         | multiplicative_expression '*' cast_expression    {
                            $$.n = ast_create_binop('*', $1.n, $3.n);
                         }
                         | multiplicative_expression '/' cast_expression    {
                            $$.n = ast_create_binop('/', $1.n, $3.n);
                         }
                         | multiplicative_expression '%' cast_expression    {
                            $$.n = ast_create_binop('%', $1.n, $3.n);
                         }
                         ;

additive_expression:
                   multiplicative_expression    { $$ = $1; }
                   | additive_expression '+' multiplicative_expression  {
                    $$.n = ast_create_binop('+', $1.n, $3.n);
                   }
                   | additive_expression '-' multiplicative_expression  {
                    $$.n = ast_create_binop('-', $1.n, $3.n);
                   }
                   ;

shift_expression:
                additive_expression { $$ = $1; }
                | shift_expression SHL additive_expression  {
                    $$.n = ast_create_binop(SHL, $1.n, $3.n);
                    STDERR_F("Bitwise shift not supported! %s:%d", 
                        yyin_name, line_num);
                }
                | shift_expression SHR additive_expression  {
                    $$.n = ast_create_binop(SHR, $1.n, $3.n);
                    STDERR_F("Bitwise shift not supported! %s:%d", 
                        yyin_name, line_num);
                }
                ;

relational_expression:
                     shift_expression   { $$ = $1; }
                     | relational_expression '<' shift_expression   {
                        $$.n = ast_create_binop('<', $1.n, $3.n);

                     }
                     | relational_expression '>' shift_expression   {
                        $$.n = ast_create_binop('>', $1.n, $3.n);
                     }
 
                     | relational_expression LTEQ shift_expression   {
                        $$.n = ast_create_binop(LTEQ, $1.n, $3.n);
                     }
 
                     | relational_expression GTEQ shift_expression   {
                        $$.n = ast_create_binop(GTEQ, $1.n, $3.n);
                     }
                     ;

equality_expression:
                   relational_expression    { $$ = $1; }
                   | equality_expression EQEQ relational_expression {
                    $$.n = ast_create_binop(EQEQ, $1.n, $3.n);
                   }
                   | equality_expression NOTEQ relational_expression    {
                    $$.n = ast_create_binop(NOTEQ, $1.n, $3.n);
                   }
                   ;

and_expression:
              equality_expression   { $$ = $1; }
              | and_expression '&' equality_expression  {
                $$.n = ast_create_binop('&', $1.n, $3.n);
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
              }
              ;

xor_expression:
              and_expression   { $$ = $1; } 
              | xor_expression '^' and_expression   {
                $$.n = ast_create_binop('^', $1.n, $3.n);
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
              }
              ;

or_expression:
             xor_expression { $$ = $1; }
             | or_expression '|' xor_expression {
                $$.n = ast_create_binop('|', $1.n, $3.n);
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
             }
             ;

logand_expression:
                 or_expression  { $$ = $1; }
                 | logand_expression LOGAND or_expression   {
                    $$.n = ast_create_binop(LOGAND, $1.n, $3.n);
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
                 }
                 ;

logor_expression:
                logand_expression   { $$ = $1; }
                | logor_expression LOGOR logand_expression  {
                    $$.n = ast_create_binop(LOGOR, $1.n, $3.n);
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
                }
                ;

conditional_expression:
                      logor_expression  { $$ = $1; }
                      | ternary_expression
                      ;

ternary_expression:
                  logor_expression '?' expression ':' conditional_expression {
                    $$.n = ast_create_ternop($1.n, $3.n, $5.n);
                    STDERR_F("Ternary operators not supported! %s:%d", 
                        yyin_name, line_num);
                  }
                  ;

assignment_expression: 
                     conditional_expression { $$ = $1; }
                     | unary_expression assignment_operator
                        assignment_expression   {   
                        if($3.n->op_type == LIST && $3.n->list.next == NULL)
                            $3.n = $3.n->list.value;
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
                   | SHLEQ      {$$.ulld = SHLEQ;
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
                   }
                   | SHREQ      {$$.ulld = SHREQ;
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
                   }
                   | ANDEQ      {$$.ulld = ANDEQ;
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
                   }
                   | XOREQ      {$$.ulld = XOREQ;
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
                   }
                   | OREQ       {$$.ulld = OREQ;
                    STDERR_F("Bitwise comparison not supported! %s:%d", 
                        yyin_name, line_num);
                   }
                   ;

expression:
          assignment_expression { $$.n = ast_list_start($1.n); }
          | expression ',' assignment_expression {
            $$.n = ast_list_insert($1.n, $3.n);
          }
          ;
/*
constant_expression:
                   conditional_expression { $$ = $1; }
                   ;
*/
declaration:
           declaration_specifiers initialized_declarator_list ';'   {
            symtab_install(current, 
                ast_create_var_decl($1.n, $2.n),
                yyin_name, line_num);
            $$.n = NULL;
           }
           | declaration_specifiers ';' {
            $$.n = NULL;
           }
           ;

declaration_specifiers:
                      storage_class_specifier   {
                        $$.n = ast_list_start($1.n);
                      }
                      | storage_class_specifier declaration_specifiers   {
                        $$.n = ast_list_insert($2.n, $1.n);
                      }
                      | type_specifier  {
                        $$.n = ast_list_start($1.n);
                      }
                      | type_specifier declaration_specifiers {
                        $$.n = ast_list_insert($2.n, $1.n);
                      }
                      | type_qualifier  {
                        $$.n = ast_list_start($1.n);
                      }
                      | type_qualifier declaration_specifiers   {
                        $$.n = ast_list_insert($2.n, $1.n);
                      }
                      ;

initialized_declarator_list:
                           initialized_declarator   { 
                            $$.n = ast_list_start($1.n); 
                           }
                           | initialized_declarator_list ',' initialized_declarator   {
                            $$.n = ast_list_insert($1.n, $3.n);
                           }
                           ;

initialized_declarator:
                      declarator { $$ = $1; } 
                      /*| declarator '=' initializer*/
                      ;

storage_class_specifier:
                       TYPEDEF  {
                        STDERR("typedef not supported! this will be tossed away");
                        $$.n = ast_create_type(TYPEDEF);
                       }
                       | EXTERN     {
                        $$.n = ast_create_type(EXTERN);
                       }
                       | STATIC     {
                        $$.n = ast_create_type(STATIC);
                       }
                       | AUTO       {
                        $$.n = ast_create_type(AUTO);
                       }
                       | REGISTER   {
                        $$.n = ast_create_type(REGISTER);
                       }
                       ;

type_specifier:
              VOID      {
                $$.n = ast_create_type(VOID);
              }
              | CHAR      {
                $$.n = ast_create_type(CHAR);
              }
              | SHORT      {
                $$.n = ast_create_type(SHORT);
              }
              | INT      {
                $$.n = ast_create_type(INT);
              }
              | LONG      {
                $$.n = ast_create_type(LONG);
              }
              | FLOAT      {
                $$.n = ast_create_type(FLOAT);
              }
              | DOUBLE      {
                $$.n = ast_create_type(DOUBLE);
              }
              | SIGNED      {
                $$.n = ast_create_type(SIGNED);
              }
              | UNSIGNED      {
                $$.n = ast_create_type(UNSIGNED);
              }
              | BOOL      {
                $$.n = ast_create_type(CHAR);
              }
              /*| struct_or_union_specifier {
                $$ = $1;
              } */ /* enum, typedef omitted */
              ;
/*
struct_or_union_specifier:  // TODO: midrule enter into struct symtab
                         struct_or_union {
                            $1.n->sue.label = NULL;
                            //ENTER_SCOPE($1.n->sue.symtab);
                         } '{' struct_declaration_list '}'    {
                            //EXIT_SCOPE();
                            $$ = $1;
                         }
                         | struct_or_union IDENT {
                            $1.n->sue.label = ast_create_ident($2);
                            symtab_install(current, 
                                ast_create_var_decl(
                                    ast_list_start($1.n),
                                    ast_list_start($2.n)
                                ), yyin_name, line_num);
                            //ENTER_SCOPE($1.n->sue.symtab);
                         } '{' struct_declaration_list '}' {
                            //EXIT_SCOPE();
                            $$ = $1; 
                         }
                         | struct_or_union IDENT    {   // incomplete
                            $1.n->sue.label = ast_create_ident($2);
                            symtab_install(current,
                                ast_create_var_decl(
                                    ast_list_start($1.n),
                                    ast_list_start($2.n)
                                ), yyin_name, line_num);
                            $$ = $1; 
                         }
                         ;

struct_or_union:
               STRUCT   {
                $$.n = ast_create_struct(current, yyin_name, line_num);
               }
               | UNION  {
                $$.n = ast_create_union(current, yyin_name, line_num);
               }
               ;

struct_declaration_list: 
                       struct_declaration   {
                        //$$.n = ast_list_start($1.n);
                       }
                       | struct_declaration_list struct_declaration {
                        //$$.n = ast_list_insert($1.n, $2.n);
                       }
                       ;

struct_declaration:
                  specifier_qualifier_list struct_declarator_list ';'   {
                    //$$.n = ast_list_merge($1.n, $2.n);
                    //symtab_install(current,
                    //    ast_create_sue_decl($1.n, $2.n),
                    //    yyin_name, line_num);
                    $$.n = NULL;
                  }
                  ;
*/
specifier_qualifier_list:
                        type_specifier  {
                            $$.n = ast_list_start($1.n);
                        }
                        | type_specifier specifier_qualifier_list   {
                            $$.n = ast_list_insert($2.n, $1.n);
                        }
                        | type_qualifier    {
                            $$.n = ast_list_start($1.n);
                        }
                        | type_qualifier specifier_qualifier_list   {
                            $$.n = ast_list_insert($2.n, $1.n);
                        }
                        ;
/*
struct_declarator_list:
                      struct_declarator {
                        $$.n = ast_list_start($1.n);
                      }
                      | struct_declarator_list ',' struct_declarator    {
                        $$.n = ast_list_insert($1.n, $3.n);
                      }
                      ;

struct_declarator:
                 declarator {
                    $$ = $1;
                 }
                 ;
*/
type_qualifier:
              CONST {
                $$.n = ast_create_type(CONST);
              }
              | RESTRICT    {
                $$.n = ast_create_type(RESTRICT);
              }
              | VOLATILE    {
                $$.n = ast_create_type(VOLATILE);
              }
              ;

/* skipped function specifiers */

declarator:
          direct_declarator {
            $$ = $1;
          }
          | pointer direct_declarator {
            $$.n = ast_list_merge($1.n, $2.n);
          }
          ;

direct_declarator:
                 IDENT  {
                    $$.n = ast_list_start(ast_create_ident($1));
                 }
                 | '(' declarator ')'   { 
                    $$ = $2; 
                 }
                 | direct_declarator '[' ']'    { 
                    $$.n = ast_list_insert($1.n, ast_create_array(NULL, NULL));
                 }
                 | direct_declarator '[' NUMBER ']' {
                    $$.n = ast_list_insert($1.n, 
                        ast_create_array(NULL, ast_create_num($3))
                    );
                 }
                 | direct_declarator '(' ident_list ')' {
                    $$.n = ast_list_start(
                        ast_create_func_call($1.n, $3.n)
                    );
                 }
                 | direct_declarator '(' ')'    {
                    $$.n = ast_list_start(
                        ast_create_func_call($1.n, NULL)
                    );
                 }  /* skipped parameter lists */
                 ;

pointer:
       '*' { 
        $$.n = ast_list_start(ast_create_ptr(NULL, NULL));
       }
       | '*' type_qualifier_list  {
        $$.n = ast_list_merge($2.n, 
            ast_list_start(ast_create_ptr(NULL, NULL)));
       }
       | '*' pointer {
        $$.n = ast_list_merge($2.n, 
            ast_list_start(ast_create_ptr(NULL, NULL)));
       }
       | '*' type_qualifier_list pointer  {
        $$.n = ast_list_merge(
            ast_list_merge($2.n, $3.n), 
                ast_list_start(ast_create_ptr(NULL, NULL)
            )
        ); 
       }
       ;


type_qualifier_list:
                   type_qualifier   {
                    $$.n = ast_list_start($1.n);
                   }
                   | type_qualifier_list type_qualifier   {
                    $$.n = ast_list_insert($1.n, $2.n);
                   }
                   ;

type_name:
         specifier_qualifier_list   {
            $$.n = $1.n;
         }
         | specifier_qualifier_list abstract_declarator  {
            $$.n = ast_list_merge($1.n, $2.n); 
         }
         ;

abstract_declarator:
                   pointer  {
                    $$ = $1;
                   }
                   | direct_abstract_declarator {
                    $$ = $1;
                   }
                   | pointer direct_abstract_declarator {
                    $$.n = ast_list_merge($1.n, $2.n);
                   }
                   ;

direct_abstract_declarator:
                          '(' abstract_declarator ')'   {
                            $$ = $2;
                          }
                          | direct_abstract_array {
                            $$.n = ast_list_start($1.n);
                          }
                          | direct_abstract_declarator direct_abstract_array {
                            $$.n = ast_list_insert($1.n, $2.n);
                          } 
                          | '(' ')' {
                            $$.n = ast_list_start(
                                ast_create_func_call(NULL, NULL)
                            ); 
                          }
                          | direct_abstract_declarator '(' ')'  {
                            $$.n = ast_list_insert($1.n, 
                                ast_create_func_call(NULL, NULL)
                            );
                          }
                          ;

direct_abstract_array:
                     '[' ']' {
                        $$.n = ast_create_array(NULL, NULL);
                     }
                     | '[' NUMBER ']'  {
                        $$.n = ast_create_array(NULL, ast_create_num($2));
                     } 
                     ;

ident_list:
          IDENT {
            $$.n = ast_list_start(ast_create_ident($1));
          }
          | ident_list ',' IDENT    {
            $$.n = ast_list_insert($1.n, ast_create_ident($3));
          }
          ;

/* skipped typedef names */

/* skipped initializers */

statement:
         labeled_statement  { $$ = $1; }
         | expression_statement { $$ = $1; }
         | selection_statement  { $$ = $1; }
         | iteration_statement  { $$ = $1; }
         | jump_statement   { $$ = $1; }
         | /*block scope*/ {
            CREATE_SCOPE(SCOPE_BLOCK);
            STDERR("Entering Compound statement");
         } compound_statement {
            // tack our scope on at the end of the list -- we knwo we're
            // entering a compound scope for the duration of this list if we see
            // this
            // this feels wrong
            $2.n = ast_list_insert($2.n,
                ast_create_compound_scope(current->previous));
            $2.n = ast_list_reverse($2.n);
            $2.n = ast_list_insert($2.n,
                ast_create_compound_scope(current));
            EXIT_SCOPE();
            STDERR("Compound statement done.")
            $$.n = $2.n;    // bison bullshit -- the empty statement is $1
         }
         ;

labeled_statement:
                 IDENT ':' statement    {   
                    $$.n = ast_create_label(ast_create_ident($1), $3.n);
                    symtab_install(current, $$.n,
                        yyin_name, line_num);
                 }
                 /* below are in SWITCtch statements ONLY
                    basically getting installed in block scope*/
                 /*| CASE constant_expression ':' statement   {
                    $$.n = ast_create_case_label($2.n, $4.n);
                    //symtab_install(current, $$.n,
                    //    yyin_name, line_num);
                 }
                 | DEFAULT ':' statement    {
                    $$.n = ast_create_default_label($3.n);
                    //symtab_install(current, $$.n, 
                    //    yyin_name, line_num);
                 }*/
                 ;

compound_statement:
                  '{' block_item_list '}'  {
                    $$.n = $2.n;
                  }
                  | '{' '}'   {
                    $$.n = NULL;
                  }
                  ;

block_item_list:
               block_item   {
                if($1.n != NULL)
                    $$.n = ast_list_start($1.n);
                else $$.n = NULL;
               }
               | block_item_list block_item {
                if($2.n != NULL)
                    $$.n = ast_list_insert($1.n, $2.n);
                else $$.n = $1.n;
               }
               ;

block_item:
          declaration   {
            $$.n = $1.n;
          }
          | statement {
            $$.n = $1.n;
          }
          ;

expression_statement:
                    ';' { 
                        $$.n = NULL;  
                    }
                    | expression ';'  { 
                        $$.n = $1.n; 
                    }
                    ;

selection_statement:
                   IF '(' expression ')' statement {
                    $$.n = ast_create_if_stmt($3.n, $5.n, NULL);
                   }
                   | IF '(' expression ')' statement ELSE statement {
                    $$.n = ast_create_if_stmt($3.n, $5.n, $7.n);
                   }
                   /*| SWITCH '(' expression ')' statement    {
                    $$.n = ast_create_switch_stmt($3.n, $5.n);
                   }*/ /* unsupported  */
                   ;

iteration_statement:
                   WHILE '(' expression ')' statement  {
                    $$.n = ast_create_while_stmt($3.n, $5.n);
                   }
                   | DO statement WHILE '(' expression ')' ';'  {
                    $$.n = ast_create_do_while_stmt($5.n, $2.n);
                   }
                   | FOR '(' for_loop_clauses ')' statement {
                    $$.n = ast_create_for_loop(
                        $3.n->list.value,
                        $3.n->list.next->list.value,
                        $3.n->list.next->list.next->list.value,
                        $5.n
                    );
                   }
                   ;    /* not supporting decls in for loop */


for_loop_clauses:   for_loop_expr ';' for_loop_expr ';' for_loop_expr {
                    $$.n = ast_list_start($5.n);
                    $$.n = ast_list_insert($$.n, $3.n);
                    $$.n = ast_list_insert($$.n, $1.n);
                }
                ;

for_loop_expr:    /*empty*/   { $$.n = NULL; }
             | expression { $$.n = $1.n->list.value; }
             ;

jump_statement:
              GOTO IDENT ';'    {
                $$.n = ast_create_goto(
                    ast_create_ident($2)
                );
              }
              | CONTINUE ';'    {
                $$.n = ast_create_type(CONTINUE);
              }
              | BREAK ';'   {
                $$.n = ast_create_type(BREAK);
              }
              | RETURN expression ';'   {
                $$.n = ast_create_return($2.n);
              }
              | RETURN ';'  {
                $$.n = ast_create_return(NULL);
              }
              ;

    /* start */
translation_unit:
                external_declaration
                | translation_unit external_declaration
                ;

external_declaration:
                    function_definition { 
                        $$ = $1;
                        
                        // add bb to list of blocks

                        struct bb *block = bb_create(&cursor);
                        if(cursor.current != NULL)
                            cursor_ingest(cursor.current);

                        cursor.current = block;
                        astprint($1.n->fndef.stmt_list);
                        
                        bb_gen_ir($1.n->fndef.stmt_list);

                        cursor_ingest(block);

                        cursor.fn_num_counter++;
                    }
                    | declaration   { $$ = $1; }
                    ;

function_definition:
                   declaration_specifiers declarator {
                    $1.n = ast_create_var_decl($1.n, 
                        ast_list_start($2.n));
                    symtab_install(current, 
                        $1.n, 
                        yyin_name, line_num);
                    CREATE_SCOPE(SCOPE_FUNCTION);
                   } compound_statement    {
                    
                    $4.n = ast_list_insert($4.n,
                        ast_create_compound_scope(current->previous));
                    $4.n = ast_list_reverse($4.n);
                    $4.n = ast_list_insert($4.n,
                        ast_create_compound_scope(current));
     
                    EXIT_SCOPE();
                    
                    $$.n = ast_create_fndef_decl($1.n, $4.n);
                   }
                   ;
/*
declaration_list:
                declaration {
                    $$.n = ast_list_start($1.n);
                }
                | declaration_list declaration  {
                    $$.n = ast_list_insert($1.n, $2.n);
                }
                ;
*/

%%

void yyerror(const char *s)
{
    STDERR("You have disturbed me almost to the point of \
insanity...There. I am insane now."); 
    STDERR_F("%s\n", s);
}
