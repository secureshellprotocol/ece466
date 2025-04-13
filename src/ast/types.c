#include <stddef.h>

#include <ast/ast.h>
#include <ast/types.h>
#include <symtab/symtab.h>

extern char yyin_name[4096];
extern int line_num;

ast_node *ast_create_type(uint32_t type_token)
{
    ast_node *n = create_node(type_token);

    switch(type_token)
    {
        case STRUCT: case UNION: case ENUM:
            n->sue.symtab = symtab_create(NULL, SCOPE_SUE,
                    yyin_name, line_num);
            break;
        default:
            break;
    }

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
