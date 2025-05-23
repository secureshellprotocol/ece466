#ifndef __AST_H_JR
#define __AST_H_JR

#include <stdint.h>

#include <lexer/lexer.lex.h>
#include <parser/grammar.tab.h>
#include <symtab/symtab.h>

#define NUM_PROTOTYPES 10

/* NODE TYPES */

typedef struct ast_node_t ast_node;

// For unary operations -- an expression is what's being acted upon by the 
//  operator. 
struct ast_node_unaop_t {
    int token;
    ast_node *expression;
};

// For binary and ternary operations, the semantics of each operation is
//  defined by what is to the left, right, and possibly center.
// eg: 2 + x, left would reference `2`, right references `x`.
struct ast_node_binop_t {
    int token;

    ast_node *left;
    ast_node *right;
};

struct ast_node_ternop_t {
    // there is only one type of ternary operator
    ast_node *left;
    ast_node *middle;
    ast_node *right;
};

struct ast_node_func_t {
    ast_node *label;
    ast_node *arglist;
};

struct ast_list_t   {
    ast_node *value;
    ast_node *prev;
    ast_node *next;
};

struct ast_node_num_t {
    union {
        unsigned long long ival;
        long double fval;
    };
    uint32_t tags;
};

struct ast_node_ident_t {
    char *value;
};

struct ast_node_char_t {
    char *value;
    int s_len;
};

struct ast_node_decl_t {
    ast_node *stgclass;
    ast_node *decl_specs;
    ast_node *declarator;
};

struct ast_node_fndef_t {
    ast_node *decl;
    ast_node *stmt_list;
};

struct ast_node_array_t {
    ast_node *to;
    ast_node *size;
};

struct ast_node_ptr_t {
    ast_node *to;
    ast_node *type_quals;
};

struct ast_node_sue_t {
    ast_node *label;
    struct symbol_scope_t *symtab;
};

struct ast_node_if_t {
    ast_node *expr;
    ast_node *stmt;
    ast_node *else_stmt;
};

struct ast_node_switch_t {
    ast_node *expr;
    ast_node *stmt;
};

struct ast_node_while_t {
    ast_node *expr;
    ast_node *stmt;
};

struct ast_node_do_while_t {
    ast_node *expr;
    ast_node *stmt;
};

struct ast_node_for_t {
    ast_node *cl1;
    ast_node *cl2;
    ast_node *cl3;
    ast_node *stmt;
};

struct ast_node_goto_t {
    ast_node *ident;
};

struct ast_node_return_t {
    ast_node *ret_expr;
};

// shared among label, case, default
struct ast_node_label_t {
    ast_node *stmt;
    ast_node *expr;
    ast_node *ident;
};

// stores a scope to go into for compound scope.
struct ast_node_compound_scope_t {
    struct symbol_scope_t *st;
};

typedef struct ast_node_t {
    int op_type;
    union {
        // terminals
        struct ast_node_ident_t ident;
        struct ast_node_num_t num;
        struct ast_node_char_t char_array;

        // expressions
        struct ast_node_unaop_t unaop;
        struct ast_node_binop_t binop;
        struct ast_node_ternop_t ternop;
        struct ast_node_func_t fncall;

        // types
        struct ast_node_decl_t d;
        struct ast_node_fndef_t fndef;
        struct ast_node_array_t array;
        struct ast_node_ptr_t ptr;
        struct ast_node_sue_t sue;
        
        // statements
        struct ast_node_if_t if_s;
        struct ast_node_switch_t switch_s;
        struct ast_node_while_t while_s;
        struct ast_node_do_while_t do_while_s;
        struct ast_node_for_t for_s;
        struct ast_node_goto_t goto_s;
        struct ast_node_return_t return_s;
        struct ast_node_label_t label_s;

        // compound scope storage
        struct ast_node_compound_scope_t cs;

        // list of nodes
        struct ast_list_t list;
    };
} ast_node;

/* FUNCTIONS */

// ast.c

ast_node *create_node(int ot);

ast_node *ast_create_ident(struct yy_struct);
ast_node *ast_create_num(struct yy_struct);
ast_node *ast_create_string(struct yy_struct);
ast_node *ast_create_charlit(struct yy_struct);
ast_node *ast_create_constant(unsigned long long int ulld);

ast_node *ast_create_unaop(int token, ast_node *e);
ast_node *ast_create_binop(int token, ast_node *l, ast_node *r);
ast_node *ast_create_ternop(ast_node *l, ast_node *m, ast_node *r);
ast_node *ast_create_func_call(ast_node *label, ast_node *arglist);

ast_node *ast_create_var_decl(ast_node *decl_specs, ast_node *decl_list);
ast_node *ast_create_fndef_decl(ast_node *decl, ast_node *stmt_list);

ast_node *ast_create_compound_scope(symbol_scope  *st);

ast_node *ast_create_struct(symbol_scope *previous, 
        char *yyin_name, unsigned int line_num);
ast_node *ast_create_union(symbol_scope *previous, 
        char *yyin_name, unsigned int line_num);

// ast_utils.c

int verify_decl_specs(ast_node *decl_specs);
uint32_t calculate_sizeof(ast_node *n);
uint32_t ast_get_type_size(ast_node *d);
uint32_t ast_get_ident_size(ast_node *ident);

// types.c

ast_node *ast_create_array(ast_node *to, ast_node *size);
ast_node *ast_create_ptr(ast_node *to, ast_node *type_quals);

// selection.c

ast_node *ast_create_if_stmt(
        ast_node *expression, ast_node *stmt, ast_node *else_stmt);
ast_node *ast_create_switch_stmt(ast_node *expression, ast_node *stmt);
ast_node *ast_create_while_stmt(ast_node *expression, ast_node *stmt);
ast_node *ast_create_do_while_stmt(ast_node *expression, ast_node *stmt);
ast_node *ast_create_for_loop(
        ast_node *cl1, ast_node *cl2, ast_node *cl3, ast_node *stmt);
ast_node *ast_create_goto(ast_node *ident);
ast_node *ast_create_return(ast_node *ret_expression);
// continue, break are handled by ast_create_type
ast_node *ast_create_label(ast_node *i, ast_node *stmt);
ast_node *ast_create_case_label(ast_node *expr, ast_node *stmt);
ast_node *ast_create_default_label(ast_node *stmt);

// ast_list.c

ast_node *ast_list_start(ast_node *start);
ast_node *ast_list_insert(ast_node *list_node, ast_node *value);
ast_node *ast_list_merge(ast_node *list_node, ast_node *list_donor);
ast_node *ast_list_reverse(ast_node *list_node);

// astprint.c

void astprint(ast_node *n);

#endif
