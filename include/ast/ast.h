#ifndef __AST_H_JR
#define __AST_H_JR

#include <lexer/lexer.lex.h>
#include <parser/op.h>

/* NODE TYPES */

typedef struct ast_node_t ast_node;

// For unary operations -- an expression is what's being acted upon by the 
//  operator. 
struct ast_node_unaop_t {
    unsigned int token;

    ast_node *expression;
};

// For binary and ternary operations, the semantics of each operation is
//  defined by what is to the left, right, and possibly center.
// eg: 2 + x, left would reference `2`, right references `x`.
typedef struct ast_node_binop_t {
    unsigned int token;

    ast_node *left;
    ast_node *right;
} nodetype_binop;

typedef struct ast_node_ternop_t {
    // there is only one type of ternary operator
    ast_node *left;
    ast_node *middle;
    ast_node *right;
} nodetype_ternop;

// Terminal nodes
struct ast_node_num_t {
    union {
        unsigned long long ival;
        long double fval;
    };
    int tags;
};

struct ast_node_ident_t {
    char *value;
};

struct ast_node_char_t {
    char *value;
    int s_len;
};

typedef struct ast_node_t {
    int op_type;
    union {
        struct ast_node_ident_t ident;
        struct ast_node_num_t num;
        struct ast_node_char_t char_array;

        struct ast_node_unaop_t unaop;
        struct ast_node_binop_t binop;
        struct ast_node_ternop_t ternop;
    };
} ast_node;

/* FUNCTIONS */

ast_node *create_node(int ot);
void free_node(ast_node *n);

ast_node *ast_create_ident(struct yy_struct);
ast_node *ast_create_num(struct yy_struct);
ast_node *ast_create_string(struct yy_struct);
ast_node *ast_create_charlit(struct yy_struct);

ast_node *ast_create_unaop(int token,
        ast_node *e);
ast_node *ast_create_binop(int token, 
        ast_node *l, ast_node *r);
ast_node *ast_create_ternop(
        ast_node *l, ast_node *m, ast_node *r);

void astprint(ast_node *n);
#endif
