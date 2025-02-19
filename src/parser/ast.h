#ifndef __AST_H_JR
#define __AST_H_JR

#include "src/lexer/lexer.lex.h"

enum op {
    IDENT=257,
    NUM,
    UNAOP,
    BINOP,
    TERNOP
}

typedef struct ast_node_t {
    int op_type;
    union {
        nodetype_ident ident;
        nodetype_num num;
        nodetype_unaop unaop;
        nodetype_binop binop;
        nodetype_ternop ternop;
    }
} ast_node;

// Token is assigned from src/lexer/tokens.h, or is a direct char referencing 
//  what the operation is doing.

// For unary operations -- an expression is what's being acted upon by the 
//  operator. 
typedef struct ast_node_unaop_t {
    unsigned int token;

    ast_node *expression;
} nodetype_unaop;

// For binary and ternary operations, the semantics of each operation is
//  defined by what is to the left, right, and possibly center.
// eg: 2 + x, left would reference `2`, right references `x`.
typedef struct ast_node_binop_t {
    char token;

    ast_node *left;
    ast_node *right;
} nodetype_binop;

typedef struct ast_node_ternop_t {
    // there is only one type of ternary operator

// Terminal nodes
typedef struct ast_node_num_t {
    unsigned long long value;
    int tags;
} nodetype_num;

typedef struct ast_node_ident_t {
    char *value;
} nodetype_ident;

#endif
