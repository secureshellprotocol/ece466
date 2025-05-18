#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <symtab/symtab.h>
#include <backend/basicblocks.h>

extern struct bb_cursor cursor;

struct bb_arg *create_arg(enum args argtype, struct bb_arg *inheritor)
{
    struct bb_arg *a = calloc(1, sizeof(struct bb_arg));

    if(inheritor != NULL)
    {
        a->size = inheritor->size;
        a->am = inheritor->am;
    }

    if(a == NULL)
    {
        ERROR("Failed to allocate a basicblock argument!");
    }

    a->at = argtype;

    switch(a->at)
    {
        case A_REG:
            a->r.rn = cursor.reg_count;
            cursor.reg_count++;
            break;
        case A_VAR: case A_IMM:
            break;
        default:
            break;
    }

    return a;
}
