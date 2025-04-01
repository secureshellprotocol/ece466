#include <stddef.h>

#include <ast/ast.h>
#include <parser/op.h>
#include <symtab/symtab.h>

void add_to_symtab(symbol_scope *current, ast_node *n)
{
    if(current == NULL)
    {
        return;
    }
    switch(n->op_type)
    {
        case FUNCTION:
            break;
        case POINTER:
            break;
        case VAR:
            break;
        // not a covered case, or invalid token/op detected
        default:
            break;
    }
}
