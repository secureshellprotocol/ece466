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

    num->size = (uint32_t) sizeof(int);
    num->am = M_LITERAL;

    return num;
}

struct bb_arg *bb_op_generate_intconst(uint32_t numarg, struct bb *block)
{
    struct bb_arg *num = create_arg(A_IMM, NULL);

    num->i.val = numarg;
    
    num->size = sizeof(int);
    num->am = M_LITERAL;

    return num;
}

struct bb_arg *bb_op_generate_ident(ast_node *n)
{
    struct bb_arg *i = create_arg(A_VAR, NULL);

    i->v.ste = symtab_lookup(current, n->ident.value, NS_IDENTS, -1);
    i->v.scope = symtab_scope_lookup(current, n->ident.value, NS_IDENTS);

    // size, am defined by generate_declarator
    // should never use this outside of that fcn

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
    o->dest = create_arg(A_REG, generate_inheritor(src1, src2));
    
//    if(ADDRTYPE(src1->am) || (ADDRTYPE(src2->am)))
//    {
//        o->dest->size = sizeof(int *);
//        o->dest->am = M_POINTER;
//        
//        if(src1->am == M_ARRAY)
//        {
//            o->dest->am = M_ARRAY;
//            o->dest->size = src1->size;
//        }
//
//        if(src2->am == M_ARRAY)
//        {
//            o->dest->am = M_ARRAY;
//            o->dest->size = src2->size;
//        }
//    }

    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_addition(struct bb_arg *src1, struct bb_arg *src2, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_ADD);
    
    o->src1 = src1;
    o->src2 = src2;
    o->dest = create_arg(A_REG, generate_inheritor(src1, src2));
    
    // Multidim is broken if you have more than 2 dims
    // todo never make a calculate_sizeof
//    if(ADDRTYPE(src1->am) || (ADDRTYPE(src2->am)))
//    {
//        o->dest->size = sizeof(int *);
//        o->dest->am = M_POINTER;
//        
//        if(src1->am == M_ARRAY)
//        {
//            o->dest->am = M_ARRAY;
//            o->dest->size = sizeof(int);
//        }
//
//        if(src2->am == M_ARRAY)
//        {
//            o->dest->am = M_ARRAY;
//            o->dest->size = sizeof(int);
//        }
//    }    
    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_div(struct bb_arg *src1, struct bb_arg *src2, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_DIV);

    o->src1 = src1;
    o->src2 = src2;
    o->dest = create_arg(A_REG, generate_inheritor(src1, src2));

    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_mod(struct bb_arg *src1, struct bb_arg *src2, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_MOD);

    o->src1 = src1;
    o->src2 = src2;
    o->dest = create_arg(A_REG, generate_inheritor(src1, src2));

    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_sub(struct bb_arg *src1, struct bb_arg *src2, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_SUB);

    o->src1 = src1;
    o->src2 = src2;
    o->dest = create_arg(A_REG, generate_inheritor(src1, src2));

    bb_op_append(o, block);

    return o->dest;
}

struct bb_arg *bb_op_generate_neg(struct bb_arg *src1, struct bb_arg *dest, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_SUB);

    o->src1 = src1;
    o->dest = dest;

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

struct bb_arg *bb_op_generate_cmp(struct bb_arg *src1, struct bb_arg *src2, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_CMP);

    o->src1 = src1;
    o->src2 = src2;

    bb_op_append(o, block);

    cursor.mode = 0;

    return NULL;
}

struct bb_arg *bb_op_generate_brlt(struct bb *block)
{
    struct bb_op *o = bb_genop(Q_BRLT);

    bb_op_append(o, block);

    return NULL;
}

struct bb_arg *bb_op_generate_brgt(struct bb *block)
{
    struct bb_op *o = bb_genop(Q_BRGT);

    bb_op_append(o, block);

    return NULL;
}

struct bb_arg *bb_op_generate_breq(struct bb *block)
{
    struct bb_op *o = bb_genop(Q_BREQ);

    bb_op_append(o, block);

    return NULL;
}

struct bb_arg *bb_op_generate_brneq(struct bb *block)
{
    struct bb_op *o = bb_genop(Q_BRNEQ);

    bb_op_append(o, block);

    return NULL;
}

struct bb_arg *bb_op_generate_jump(struct bb *block)
{
    struct bb_op *o = bb_genop(Q_JUMP);
    
    bb_op_append(o, block);

    return NULL;
}

struct bb_arg *bb_op_generate_return(struct bb_arg *src1, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_RETURN);

    o->src1 = src1;
    bb_op_append(o, block);

    return NULL;
}

struct bb_arg *bb_op_generate_arg(ast_node *argval, uint32_t arg, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_ARG);

    o->src1 = bb_gen_ir(argval);
    o->src2 = bb_op_generate_intconst(arg, block);

    bb_op_append(o, block);

    return NULL;
}

struct bb_arg *bb_op_generate_call(struct bb_arg *l, uint32_t arg, struct bb *block)
{
    struct bb_op *o = bb_genop(Q_CALL);

    o->src1 = l;
    o->src2 = bb_op_generate_intconst(arg, block);
    o->dest = create_arg(A_REG, NULL);
    
    // assuming int retval
    o->dest->am = M_LITERAL;
    o->dest->size = sizeof(int);
    
    bb_op_append(o, block);

    return NULL;
}

// this was a brainrotten attempt to somehow carry size information, for
// calculating multidim arrays, and to know whether a register points to an
// array, pointer, or "literal" (int, char, etc).
// its bugged, in that it cant do multidim arrays. its good duct tape for
// knowing when to lea vs load. if given more time, i would cast this entire
// data structure into the depths of hell and just go back and implement sizeof
// properly. oh well! truly bestows this as a "cringepiler"
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
            if(d->list.value->array.size == NULL)
            {
                STDERR("Cant have incomplete arrays!");
                return NULL;
            }

            if(a->am == M_ARRAY) //multidim
            {
                a->size = d->list.value->array.size->num.ival * a->size;
                return a;
            }

            a->size = a->size;
            a->am = M_ARRAY;
            // need to generate pointer to access memory at that array
            // consider: do we need M_ARRAY?
            return bb_op_generate_lea(
                    a, create_arg(A_REG, a), block
                    );
            break;
        case POINTER:
            a = bb_op_generate_declarators(d->list.next, block);
            if(a->am == M_ARRAY) //multidim
                return a;
            
            a->am = M_POINTER;
            a->size = sizeof(int *);
            return a;
            break;
        case IDENT:
            a = bb_op_generate_ident(d->list.value);
            a->size = sizeof(int);
            a->am = M_LITERAL;
            return a;
            break;
        case FUNCTION:
            a = bb_op_generate_ident(d->list.value->fncall.label->list.value);
            a->size = sizeof(int);
            a->am = M_LITERAL;      // these are wrong and placeholders
            return a;
            break;
        default:
            STDERR("\n\nDunno what to do with this");
            astprint(d->list.value);
            break;  
    }
    STDERR("Failed to generate declarators!");
    return NULL;
}
