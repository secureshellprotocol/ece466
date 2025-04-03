#include <ast/ast.h>
#include <ast/types.h>

ast_node *ast_create_type(uint32_t type_token)
{
    ast_note *n = create_node(type_token);

    return n;
}

ast_node *ast_create_array(ast_node *s)
{
    ast_node *a = create_node(ARRAY);
    a->size = s;

    return a;
}

