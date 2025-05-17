#include <stdlib.h>

#include <james_utils.h>
#include <backend/basicblocks.h>
#include <parser/op.h>

extern symbol_scope *current;

extern struct bb_cursor cursor;

#define ENTER_SCOPE(scope)\
    current = scope;

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

// recursively generate IR from AST nodes 
// generates and returns arguments, and appends ops to the block
struct bb_arg *bb_gen_ir(ast_node *n, struct bb *block)
{
    if(n == NULL)
    {
        STDERR("Null AST Node encountered!")
        return NULL;
    }

    //STDERR_F("Found %d", n->op_type);

    switch(n->op_type)
    {
        // List items -- we treat them individually, in order, then move on
        case LIST: // keep on moving
            if(bb_gen_ir(n->list.value, block) != 0)
            {
                STDERR("Failure while entering LIST!");
            }
            if(n->list.next != NULL)
            {
                bb_gen_ir(n->list.next, block);
            }
            //STDERR("Finished processing list");
            break;
        case COMPOUND_SCOPE: // scope promotion
            STDERR("Entering scope");
            ENTER_SCOPE(n->cs.st);
            break;
        // each op gets descended, fill out their arguments, then we fill them
        // out
        case UNAOP:
            {
                struct bb_arg *a = bb_gen_ir(n->unaop.expression, block);
                switch(n->unaop.token)
                {
                    case '*':
                        return bb_op_generate_load(
                                    bb_op_generate_lea(a, create_arg(A_REG, NULL), block),
                                    create_arg(A_REG, NULL), block
                                );
                        break;
                    default:
                        STDERR_F("Unhandled op \'%c\' when generating unaop basic block", 
                                n->unaop.token);
                        goto error;
                }
            }
            break;
        case BINOP:
            {
                struct bb_arg *r = bb_gen_ir(n->binop.right, block);
                struct bb_arg *l = bb_gen_ir(n->binop.left, block);

                switch(n->binop.token)
                {
                    case '=':   // src1, dest 
                        return bb_op_generate_mov(r, l, block); 
                    case '+':   // src1, src2
                        return bb_op_generate_addition(l, r, block);
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
                
                //astprint(si->d);

                ast_node *sin = si->d->d.declarator;
                     
                return bb_op_generate_ident(n); // joe schmoe primitive variable
                //return bb_op_generate_declarators(sin, block);
            }
            break;

        default:    // joever
            STDERR_F("Failed to generate IR for BB type %d! Have:", n->op_type);
            astprint(n);
            goto error;
    }

error:
    return NULL;
}
