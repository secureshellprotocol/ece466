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

    block->t = NULL;
    block->f = NULL;

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

//void cursor_ingest(struct bb *block)
void cursor_ingest()
{
    struct bb *block = cursor.current;
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
    
    cursor.current = NULL;

    return;
}

struct bb_arg *bb_gen_condexpr(ast_node *c, struct bb *root, 
        struct bb *t, struct bb *f)
{
    bb_gen_ir(c);   // generates cmp, relevant br**
    root->t = t;     // filled in later
    root->f = f; // filled in later
    
    return NULL;
}

struct bb *bb_gen_if(ast_node *c, struct bb *root)
{
    struct bb *t = bb_create(&cursor);
    struct bb *f = bb_create(&cursor);
    struct bb *n = f;   // by default, our false arm is fallthrough

    if(c->if_s.else_stmt != NULL)
    {
        f = bb_create(&cursor);     // we have an else cond
    }

    // finish off root block, generate condexprs and branches 
    bb_gen_condexpr(c->if_s.expr, root, t, f);

    cursor_ingest(root);    // root is done

    if(c->if_s.stmt == NULL)
    {
        STDERR("Empty IF statement!");
        return NULL;
    }
    cursor.current = t;
    bb_gen_ir(c->if_s.stmt);
    t->t = n;
    cursor_ingest(t);



    if(c->if_s.else_stmt != NULL)
    {
        cursor.current = f;
        bb_gen_ir(c->if_s.else_stmt);
        f->t = n;
        cursor_ingest(f);
    }

    // current cursor is NULL

    // unfinished, need to somehow end the root and start linking the rest of
    // the ast with the next `n` bb
    // currently deprecating the recursive pass of the basic block -- in
    // bb_gen_ir, it will refer to the current block held by the cursor. THIS
    // CURRENT MEANS THAT WE HAVENT INGESTED IT YET. Ingestion should involve
    // setting current to null, and then we define the next block.
    return n;
}

// recursively generate IR from AST nodes 
// generates and returns arguments, and appends ops to the block
// Null-terminates -- last 'arg' should be a null arg
//struct bb_arg *bb_gen_ir(ast_node *n, struct bb *block)
struct bb_arg *bb_gen_ir(ast_node *n)
{
    //struct bb *block = cursor.current;

    if(n == NULL)
    {
        STDERR("Null AST Node encountered!")
        return NULL;
    }

    switch(n->op_type)
    {
        // List items -- we treat them individually, in order, then move on
        case LIST: // keep on moving
            {
                struct bb_arg *a = bb_gen_ir(n->list.value);
                if(n->list.next != NULL)
                {
                    bb_gen_ir(n->list.next);
                }
                return a;
            }
            break;
        case COMPOUND_SCOPE: // scope promotion
            STDERR("Entering scope");
            ENTER_SCOPE(n->cs.st);
            break;
        case IF:
            {
                cursor.current = bb_gen_if(n, cursor.current);
                break;
            }
        case UNAOP:
            {
                struct bb_arg *a = bb_gen_ir(n->unaop.expression);
                if(a == NULL)
                    goto error;
                switch(n->unaop.token)
                {
                    case '*':
                        if(LEFTOP)
                            return a;
                        return bb_op_generate_load(
                                    a, create_arg(A_REG, NULL), cursor.current
                                );
                        break;
                    case '&':
                        if(LEFTOP)
                            return a;
                        return bb_op_generate_mov(
                                    a, create_arg(A_REG, NULL), cursor.current
                                );
                        break;
                    case PLUSPLUS:
                        STDERR("PLUSPLUS Unhandled");
                        break;
                    case MINUSMINUS:
                        STDERR("MINUSMINUS Unhandled");
                        break;
                    //case '-':
                    //    return bb_op_generate_neg(
                    //                a, create_arg(A_REG, NULL), cursor.current
                    //            );
                    case '~': 
                        STDERR("Bitwise operations not supported!");
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
                switch(n->binop.token)
                {
                    // assignment
                    case '=':   // src1, dest
                        {
                            LEFTOP_MODE(); 
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            RIGHTOP_MODE();
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            if( (LITERALTYPE(l->am)) != (LITERALTYPE(r->am)) )
                            {
                                STDERR("Incompatible assignment!");
                            }
                            if(ADDRTYPE(l->am))
                                return bb_op_generate_store(r, l, cursor.current);
                            return bb_op_generate_mov(r, l, cursor.current); 
                        }
                    case '&': case '|': case '^':
                        STDERR("Bitwise operations not supported!");
                        break;
                    // expressions
                    case TYPECAST:
                        STDERR("TYPECAST not supported!");
                        break;
                    // arithmetic
                    case '+':   // src1, src2
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;

                            if((ADDRTYPE(l->am)) || (ADDRTYPE(r->am)))
                            {
                                if(LITERALTYPE(l->am))
                                {
                                    l = bb_op_generate_mul(l, 
                                            bb_op_generate_intconst(r->size, cursor.current),
                                            cursor.current);
                                }
                                if(LITERALTYPE(r->am))
                                {
                                    r = bb_op_generate_mul(r, 
                                            bb_op_generate_intconst(l->size, cursor.current),
                                            cursor.current);
                                }
                            }

                            return bb_op_generate_addition(l, r, cursor.current);
                        }
                    case '*':
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;

                            return bb_op_generate_mul(l, r, cursor.current);
                        }
                    case '/':
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;
                            
                            return bb_op_generate_div(l, r, cursor.current);
                        }
                    case '%':
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;
                            
                            return bb_op_generate_mod(l, r, cursor.current);
                        }
                    case '-':
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;
                            
                            return bb_op_generate_sub(l, r, cursor.current);
                        }
                    case SHL: case SHR:
                        STDERR("Bitwise shifts not supported!");
                        break;
                    // comparisons
//                    case '<':
//                        {
//                            struct bb_arg *r = bb_gen_ir(n->binop.right);
//                            struct bb_arg *l = bb_gen_ir(n->binop.left);
//                            if(r == NULL || l == NULL)
//                                goto error;
//                            
//                            struct bb_arg *res = bb_op_generate_cmp(l, r, cursor.current);
//                            return bb_op_generate_brlt(res, cursor.current);
//                        }
//                    case '>':
//                        {
//                            struct bb_arg *r = bb_gen_ir(n->binop.right);
//                            struct bb_arg *l = bb_gen_ir(n->binop.left);
//                            if(r == NULL || l == NULL)
//                                goto error;
//                            
//                            struct bb_arg *res = bb_op_generate_cmp(l, r, cursor.current);
//                            return bb_op_generate_brgt(res, cursor.current);
//                        }
//                    case LTEQ:
//                        {
//                            struct bb_arg *r = bb_gen_ir(n->binop.right);
//                            struct bb_arg *l = bb_gen_ir(n->binop.left);
//                            if(r == NULL || l == NULL)
//                                goto error;
//                           
//                            // conditional inversion
//                            struct bb_arg *res = bb_op_generate_cmp(r, l, cursor.current);
//                            return bb_op_generate_brgt(res, cursor.current);
//                        }
//                    case GTEQ:
//                        {
//                            struct bb_arg *r = bb_gen_ir(n->binop.right);
//                            struct bb_arg *l = bb_gen_ir(n->binop.left);
//                            if(r == NULL || l == NULL)
//                                goto error;
//                            
//                            // conditional inversion
//                            struct bb_arg *res = bb_op_generate_cmp(r, l, cursor.current);
//                            return bb_op_generate_brlt(res, cursor.current);
//                        }
//                    case EQEQ:
//                        {
//                            struct bb_arg *r = bb_gen_ir(n->binop.right);
//                            struct bb_arg *l = bb_gen_ir(n->binop.left);
//                            if(r == NULL || l == NULL)
//                                goto error;
//                            
//                            struct bb_arg *res = bb_op_generate_cmp(l, r, cursor.current);
//                            return bb_op_generate_breq(res, cursor.current);
//                        }
//                    case NOTEQ:
//                        {
//                            struct bb_arg *r = bb_gen_ir(n->binop.right);
//                            struct bb_arg *l = bb_gen_ir(n->binop.left);
//                            if(r == NULL || l == NULL)
//                                goto error;
//                            
//                            struct bb_arg *res = bb_op_generate_cmp(l, r, cursor.current);
//                            return bb_op_generate_brneq(res, cursor.current);
//                        }
//
                    default:
                        STDERR_F("Unhandled op \'%c\' when generating binop basic block", 
                                n->binop.token);
                        goto error;
                }
            }
            break;
        case TERNOP:
            STDERR("Ternary operators not supported!");
            return NULL;
            break;
        // each expression is handled in place
        case NUMBER:
            return bb_op_generate_constant(n, cursor.current);
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
                     
                return bb_op_generate_declarators(sin, cursor.current);
            }
            break;

        default:    // joever
            STDERR_F("Failed to generate IR for BB type %d! Have:", n->op_type);
            astprint(n);
            goto error;
    }

error:  // we are done -- null terminated
    return NULL;
}
