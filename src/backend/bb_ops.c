#include <stdlib.h>

#include <james_utils.h>
#include <lexer/lexer.lex.h>
#include <backend/basicblocks.h>
#include <ast/ast.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>
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
    struct bb_arg *num = create_arg(A_IMM, NULL);

    // fun game: try saying this fifteen times in 60 seconds!
    num->i.val = n->num.ival;

    return num;
}

struct bb_arg *bb_op_generate_intconst(uint32_t num, struct bb *block)
{
    struct bb_arg *numarg = create_arg(A_IMM, NULL);

    numarg->i.val = num;

    return numarg;
}

struct bb_arg *bb_op_generate_ident(ast_node *n)
{
    struct bb_arg *i = create_arg(A_VAR, NULL);

    i->v.ste = symtab_lookup(current, n->ident.value, NS_IDENTS, -1);
    i->v.scope = symtab_scope_lookup(current, n->ident.value, NS_IDENTS);

    i->size = calculate_sizeof(i->v.ste->d);

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

struct bb_arg *bb_op_generate_store(struct bb_arg *src, struct bb_arg *dest, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_STORE);

    o->src1 = src;
    o->dest = dest;

    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_mul(struct bb_arg *src1, struct bb_arg *src2, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_MULT);

    o->src1 = src1;
    o->src2 = src2;
    o->dest = create_arg(A_REG, NULL);

    bb_op_append(o, block);

    return o->dest;

}

struct bb_arg *bb_op_generate_addition(struct bb_arg *l, struct bb_arg *r, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_ADD);

    switch(l->at)
    {
        case A_VAR:
            l = bb_op_generate_mov(l, create_arg(A_REG, l), block);
            break;
        default:
            break;
    }
    
    //if(IS_MODE_INDIR())
    //{
    //    r = bb_op_generate_mul(r,
    //            bb_op_generate_intconst(l->size, block), block
    //            );
    //}
    
//    switch(r->at)
//    {
//        case A_VAR:
//            r = bb_op_generate_mov(r, create_arg(A_REG, r), block);
//            break;
//        default:
//            break;
//    }
   
    

//    if(IS_ADDR(r->mode))
//    {
//        STDERR_F("POO %d", l->size);
//
//        r = bb_op_generate_mul(r, 
//                bb_op_generate_intconst(l->size, block), block
//                );
//    }
//    if(IS_ADDR(l->mode))
//    {
//        STDERR_F("POO %d", r->size);
//
//        l = bb_op_generate_mul(l, 
//                bb_op_generate_intconst(r->size, block), block
//                );
//    }

    o->src1 = l;
    o->src2 = r;
    o->dest = create_arg(A_REG, NULL);

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

    // check if we need array stuff

    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_declarators(ast_node *d, struct bb *block)
{
    if(d == NULL)
    {
        STDERR("Received NULL declarator!");
        return NULL;
    }

    if(d->list.value == NULL)
    {
        STDERR("Declarator contains NULL List item!");
        return NULL;
    }

    struct bb_arg *a;
    switch(d->list.value->op_type)
    {
        case ARRAY:
            a = bb_op_generate_declarators(d->list.next, block);
            return bb_op_generate_lea(
                    a, create_arg(A_REG, a), block
                    );
            break;
        case POINTER:
            a = bb_op_generate_declarators(d->list.next, block);
            return bb_op_generate_load(
                    a, create_arg(A_REG, a), block
                    );
            break;
        case IDENT:
            return bb_op_generate_ident(d->list.value);
            break;
        default:
            STDERR("Dunno what to do with this");
            astprint(d->list.value);
            break;  
    }
    return NULL;
}
