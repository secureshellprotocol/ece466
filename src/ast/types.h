#ifndef __JR_AST_TYPES_H
#define __JR_AST_TYPES_H

#include <stdint.h>

#include <ast/ast.h>
#include <parser/op.h> 

struct type_array {
    ast_node *size;
};

struct type_fcn {
    ast_node *label;
    ast_node *list;
};

struct ast_node_type_t {
    union {
        struct type_array a;
    };
};

ast_node *ast_create_type(uint32_t type_token);
ast_node *ast_create_array(ast_node *s);

#endif
