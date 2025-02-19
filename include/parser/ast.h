#ifndef __AST_H_JR
#define __AST_H_JR

#include <lexer/lexer.lex.h>
#include <lexer/tokens.h>
#include <parser/op.h>

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
typedef struct ast_node_num_t {
    union {
        unsigned long long ival;
        long double fval;
    };
    int tags;
} nodetype_num;

typedef struct ast_node_ident_t {
    char *value;
} nodetype_ident;


/* ast node */

typedef struct ast_node_t {
    int op_type;
    union {
        struct ast_node_ident_t ident;
        struct ast_node_num_t num;
        struct ast_node_unaop_t unaop;
        struct ast_node_binop_t binop;
        struct ast_node_ternop_t ternop;
    };
} ast_node;


ast_node *create_node(int ot);
void print_from_node(ast_node *n);

#endif
