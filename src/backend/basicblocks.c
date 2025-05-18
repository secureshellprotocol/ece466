#include <stdlib.h>

#include <james_utils.h>
#include <backend/basicblocks.h>
#include <parser/op.h>

extern symbol_scope *current;

extern struct bb_cursor cursor;

#define ENTER_SCOPE(scope)\
    current = scope;

#define LEFTOP_MODE()\
    cursor.mode = 1;

#define RIGHTOP_MODE()\
    cursor.mode = 0;

struct bb *bb_create(struct bb_cursor *cursor)
{
    struct bb *block = calloc(1, sizeof(struct bb));

    block->fn_num = cursor->fn_num_counter;
    block->bb_num = cursor->bb_num_counter;

    block->start = NULL;

    block->next = NULL;

    return block;
}

void bb_op_append(struct bb_op *op, struct bb *block)
{
    struct bb_op *tail = block->start;   // start list head
   
    // check if the block is empty
    //  fill it in if true
    if(tail == NULL)
    {
        block->start = op;
        return;
    }

    while(tail->next != NULL)
    {
        tail = tail->next;
    }
    
    // end of op list
    tail->next = op;
    return;
}

void cursor_ingest(struct bb *block)
{
    if(block == NULL)
    {
        STDERR("Given null block!");
        return;
    }

    struct bb *tail = cursor.head;

    if(tail == NULL)
    {
        cursor.head = block;
        return;
    }

    while(tail->next != NULL)
    {
        tail = tail->next;
    }

    tail->next = block;
    return;

}

struct bb_arg *bb_gen_condexpr(ast_node *c, struct bb *t, struct bb *f);

struct bb_arg *bb_gen_if(ast_node *c)
{
    
}

// recursively generate IR from AST nodes 
// generates and returns arguments, and appends ops to the block
struct bb_arg *bb_gen_ir(ast_node *n, struct bb *block)
{
    if(n == NULL)
    {
        STDERR("Null AST Node encountered!")
        return NULL;
    }

    switch(n->op_type)
    {
        // List items -- we treat them individually, in order, then move on
        case LIST: // keep on moving
            struct bb_arg *a = bb_gen_ir(n->list.value, block);
            if(n->list.next != NULL)
            {
                bb_gen_ir(n->list.next, block);
            }
            return a;
            break;
        case COMPOUND_SCOPE: // scope promotion
            STDERR("Entering scope");
            ENTER_SCOPE(n->cs.st);
            break;
        case IF:
            {
                struct bb *t, *f;

                

                bb_gen_condexpr(c, block);


            }
        case UNAOP:
            {
                struct bb_arg *a = bb_gen_ir(n->unaop.expression, block);
                switch(n->unaop.token)
                {
                    case '*':
                        if(LEFTOP)
                            return a;
                        if(ADDRTYPE(a->am))
                            return a;
                        return bb_op_generate_load(
                                    a, create_arg(A_REG, NULL), block
                                );
                        break;
                    case '&':
                        if(LEFTOP)
                            return a;
                        return bb_op_generate_mov(
                                    a, create_arg(A_REG, NULL), block
                                );
                    default:
                        STDERR_F("Unhandled op \'%c\' when generating unaop basic block", 
                                n->unaop.token);
                        goto error;
                }
            }
            break;
        case BINOP:
            {
                switch(n->binop.token)
                {
                    case '=':   // src1, dest
                        {
                            LEFTOP_MODE(); 
                            struct bb_arg *l = bb_gen_ir(n->binop.left, block);
                            RIGHTOP_MODE();
                            struct bb_arg *r = bb_gen_ir(n->binop.right, block);
                            if( (LITERALTYPE(l->am)) != (LITERALTYPE(r->am)) )
                            {
                                STDERR("Incompatible assignment!");
                            }
                            if(ADDRTYPE(l->am))
                                return bb_op_generate_store(r, l, block);
                            return bb_op_generate_mov(r, l, block); 
                        }
                    case '+':   // src1, src2
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right, block);
                            struct bb_arg *l = bb_gen_ir(n->binop.left, block);
                            if(r == NULL || l == NULL)
                                goto error;

                            if((ADDRTYPE(l->am)) || (ADDRTYPE(r->am)))
                            {
                                if(LITERALTYPE(l->am))
                                {
                                    l = bb_op_generate_mul(l, 
                                            bb_op_generate_intconst(r->size, block),
                                            block);
                                }
                                if(LITERALTYPE(r->am))
                                {
                                    r = bb_op_generate_mul(r, 
                                            bb_op_generate_intconst(l->size, block),
                                            block);
                                }
                            }

                            return bb_op_generate_addition(l, r, block);
                        }

                    default:
                        STDERR_F("Unhandled op \'%c\' when generating binop basic block", 
                                n->binop.token);
                        goto error;
                }
            }
            break;
        // each expression is handled in place
        case NUMBER:
            return bb_op_generate_constant(n, block);
            break;
        case IDENT:
            {
                symtab_elem *si = symtab_lookup(current, n->ident.value, NS_IDENTS, -1);
                if(si == NULL)
                {
                    STDERR_F("Couldnt find %s in symtab!", n->ident.value);
                    return NULL;
                }
                
                ast_node *sin = si->d->d.declarator;
                     
                return bb_op_generate_declarators(sin, block);
            }
            break;

        default:    // joever
            STDERR_F("Failed to generate IR for BB type %d! Have:", n->op_type);
            astprint(n);
            goto error;
    }

    return 

error:
    return NULL;
}
