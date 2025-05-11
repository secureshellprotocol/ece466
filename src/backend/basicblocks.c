#include <stdlib.h>

#include <james_utils.h>
#include <backend/basicblocks.h>
#include <parser/op.h>


struct bb *bb_create(int fn_num, int number)
{
    struct bb *block = calloc(1, sizeof(struct bb));

    block->fn_num = fn_num;
    block->num = number;

    block->start = NULL;

    return block;
}

struct bb *bb_op_prepend(struct bb_op *op, struct bb *block)
{
    struct bb_op *tail = op->next;
    
    while(tail->next != NULL)
    {
        tail = tail->next;
    }
    
    // end of op list
    tail->next = block->start;
    block->start = op;

    return block;
}

// recursively generate IR from AST nodes 
int bb_gen_ir(symbol_scope *s, ast_node *n, struct bb block)
{
    if(n == NULL)
    {
        return -1;
    }

    switch(n->op_type)
    {
        // List items -- we treat them individually, in order, then move on
        case LIST: // keep on moving
            if(bb_gen_ir(s, n->list.next, block) != 0)
            {
                STDERR("Failure while entering LIST!");
            }
            break;
        // each op gets descended, fill out their arguments, then we fill them
        // out
        case UNAOP:
            bb_gen_ir(s, n->unaop.expression, block);
            switch(n->unaop.token)
            {
                
            }
            break;
        case BINOP:
            bb_op_prepend(Q_INCOMPLETE, block);

            bb_gen_ir(s, n->binop.left, block);
            bb_gen_ir(s, n->binop.right, block);

            switch(n->binop.token)
            {
                case '=':
                    break;
                default:
                    STDERR_F("Unhandled op \'%c\' when generating binop basic block", 
                            n->binop.token);
                    goto error;
            }
            break;
        // each expression is handled in place
        case NUMBER:
            if(block->)
            struct bb_arg a_num = bb_arg_create_number(n);

            break;
            //joever
        default:    // put a printer here
            STDERR_F("Failed to generate IR for BB type %d! Have:", n->op_type);
            astprint(n);
            goto error;
    }

    return 0;
error:
    return -1;
}

struct bb_arg *create_arg(enum args argtype)
{
    struct bb_arg *a = calloc(1, sizeof(struct bb_arg));

    a->at = argtype;

    return a;
}
