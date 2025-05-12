#include <stdlib.h>

#include <james_utils.h>
#include <backend/basicblocks.h>
#include <ast/ast.h>
#include <symtab/symtab.h>

extern symbol_scope *current;

extern struct bb_cursor cursor;

struct bb_op *bb_genop(enum quadtypes qt)
{
    struct bb_op *op = calloc(1, sizeof(struct bb_op));

    if(op == NULL)
    {
        ERROR("FAILED TO ALLOCATE BASIC BLOCK OP");
    }

    op->qt = qt;

    op->src1 = NULL;
    op->src2 = NULL;
    op->dest = NULL;
    
    op->next = NULL;
    
    return op;
}

struct bb_arg *bb_op_generate_constant(ast_node *n, struct bb *block)
{
    struct bb_arg *num = create_arg(A_IMM);

    // fun game: try saying this fifteen times in 60 seconds!
    num->i.val = n->num.ival;

    return num;
}

struct bb_arg *bb_op_generate_ident(ast_node *n)
{
    struct bb_arg *i = create_arg(A_VAR);

    i->v.ste = symtab_lookup(current, n->ident.value, NS_IDENTS);
    i->v.scope = symtab_scope_lookup(current, n->ident.value, NS_IDENTS);

    // to be inserted

    return i;
}

struct bb_arg *bb_op_generate_mov(struct bb_arg *src, struct bb_arg *dest, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_MOV);

    o->src1 = src;
    o->dest = dest;

    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_addition(struct bb_arg *l, struct bb_arg *r, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_ADD);

    switch(l->at)
    {
        case A_VAR:
            l = bb_op_generate_mov(l, create_arg(A_REG), block);
        default:
            break;
    }

    switch(r->at)
    {
        case A_VAR:
            r = bb_op_generate_mov(r, create_arg(A_REG), block);
        default:
            break;
    }
    
    o->src1 = l;
    o->src2 = r;
    o->dest = create_arg(A_REG);

    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_load(struct bb_arg *src1, struct bb_arg *dest, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_LOAD);

    o->src1 = src1;
    o->dest = dest;

    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_lea(struct bb_arg *src1, struct bb_arg *dest, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_LEA);

    o->src1 = src1;
    o->dest = dest;

    bb_op_append(o, block);

    return o->dest;
}
