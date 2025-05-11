#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <symtab/symtab.h>
#include <backend/basicblocks.h>

struct bb_arg *create_arg(enum args argtype)
{
    struct bb_arg *a = calloc(1, sizeof(struct bb_arg));

    if(a == NULL)
    {
        ERROR("Failed to allocate a basicblock argument!");
    }

    a->at = argtype;

    return a;
}

struct bb_arg *bb_arg_create_number(ast_node *number)
{
    if(number == NULL) {
        STDERR("NULL VALUE PASSED");
        goto error;
    }
    
    if(number->op_type != NUMBER)
    {
        STDERR("This isnt a number! This is...");
        astprint(number);
        goto error;
    }

    struct bb_arg *a_num = create_arg(A_IMM);

    a_num->i.val = number->num.ival;

    return a_num;
error:
    return NULL;
}
