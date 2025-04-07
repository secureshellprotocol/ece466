#include <ast/ast.h>
#include <ast/types.h>

ast_node *ast_create_type(uint32_t type_token)
{
    ast_node *n = create_node(type_token);

    return n;
}

ast_node *ast_create_array(ast_node *to, ast_node *s)
{
    ast_node *a = create_node(ARRAY);
    a->array.to = to;
    a->array.size = s;

    return a;
}

ast_node *ast_create_ptr(ast_node *to, ast_node *type_quals)
{
    ast_node *p = create_node(POINTER);
    p->ptr.to = to;
    p->ptr.type_quals = type_quals;

    return p;
}
