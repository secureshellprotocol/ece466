#include <stdlib.h>

#include <backend/basicblocks.h>

struct bb_op *bb_genop(enum quadtypes qt)
{
    struct bb_op *op = calloc(1, sizeof(struct bb_op));

    if(op == NULL)
    {
        ERROR("FAILED TO ALLOCATE BASIC BLOCK OP");
    }

    op->src1 = NULL;
    op->src2 = NULL;
    op->dest = NULL;
    
    op->next = NULL;
    
    return op;
}

void bb_generate_assignment(struct bb block)
{
}
