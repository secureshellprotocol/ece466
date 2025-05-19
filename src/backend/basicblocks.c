#include <stdlib.h>

#include <james_utils.h>
#include <backend/basicblocks.h>
#include <parser/op.h>

extern symbol_scope *current;

extern struct bb_cursor cursor;

#define ENTER_SCOPE(scope)\
    current = scope;

#define CONDEXPR_MODE()\
    cursor.mode = 2;

#define LEFTOP_MODE()\
    cursor.mode = 1;

#define RIGHTOP_MODE()\
    cursor.mode = 0;

struct bb *bb_create(struct bb_cursor *cursor)
{
    struct bb *block = calloc(1, sizeof(struct bb));

    block->fn_num = cursor->fn_num_counter;
    block->bb_num = cursor->bb_num_counter;
    
    // assuming this is for the same function.
    //      this gets reset upon entry to a new function, within
    //      src/parser/grammar.y
    cursor->bb_num_counter++;

    block->start = NULL;

    block->next = NULL;

    block->t = NULL;
    block->f = NULL;

    return block;
}

void bb_op_append(struct bb_op *op, struct bb *block)
{
    if(block == NULL)
    {
        STDERR("Given a null block while generating op!");
        return;
    }

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
        STDERR("Current BB is NULL!!!");
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

struct bb *bb_gen_if(ast_node *c, struct bb *root)
{
    struct bb *t = bb_create(&cursor);
    struct bb *f = bb_create(&cursor);
    struct bb *n = f;   // by default, our false arm is fallthrough

    if(c->if_s.else_stmt != NULL)
    {
        f = bb_create(&cursor);     // we have an else cond
    }

    if(c && c->if_s.expr == NULL)
    {
        STDERR("IF Statement has empty expression!");
    }

    // TODO: fill in && ||

    // finish off root block, generate condexprs and branches
    CONDEXPR_MODE();    
    struct bb_arg *a = bb_gen_ir(c->if_s.expr);       // generates cmp, relevant br**
                                   // TODO: if(x) like expressions dont work

    if(cursor.mode == 2)    // still in condexpr mode, never emitted cmp
    {
        bb_op_generate_cmp(a, 
                bb_op_generate_intconst(1, cursor.current), 
                cursor.current);
        bb_op_generate_breq(cursor.current);
        RIGHTOP_MODE();
    }
    root->t = t;        // filled in later
    root->f = f;        // filled in later
    cursor_ingest();    // root is done

    if(c->if_s.stmt == NULL)
    {
        STDERR("Empty IF statement!");
        return NULL;
    }
 
    // generate true arm
    cursor.current = t;
    bb_gen_ir(c->if_s.stmt);
 
    // inject a jump to n
    bb_op_generate_jump(cursor.current);
    cursor.current->t = n;
    cursor_ingest();

    if(c->if_s.else_stmt != NULL)
    {
        cursor.current = f;
        bb_gen_ir(c->if_s.else_stmt);
        
        bb_op_generate_jump(cursor.current);
        cursor.current->t = n;
        cursor_ingest();
    }

    // current cursor is NULL

    // currently deprecating the recursive pass of the basic block -- in
    // bb_gen_ir, it will refer to the current block held by the cursor. THIS
    // CURRENT MEANS THAT WE HAVENT INGESTED IT YET. Ingestion should involve
    // setting current to null, and then we define the next block.
    return n;
}

struct bb *bb_gen_while(ast_node *c, struct bb *root)
{
    struct bb *body = bb_create(&cursor);
    struct bb *cp = bb_create(&cursor);     // continue point
    struct bb *bp = bb_create(&cursor);     // break point

    // inject a jump from root to our continue point
    bb_op_generate_jump(root);
    root->t = cp;
    cursor_ingest(); // eat up root

    // produce cp
    cursor.current = cp;
    CONDEXPR_MODE();
    struct bb_arg *a = bb_gen_ir(c->while_s.expr);
    if(cursor.mode == 2)    // still in condexpr mode, never emitted cmp
    {
        bb_op_generate_cmp(a, 
                bb_op_generate_intconst(1, cursor.current), 
                cursor.current);
        bb_op_generate_breq(cursor.current);
        RIGHTOP_MODE();
    }

    cursor.current->t = body;
    cursor.current->f = bp;
    cursor_ingest();

    // produce body
    cursor.current = body;
    cursor.cp = cp;
    cursor.bp = bp;
    bb_gen_ir(c->while_s.stmt);
    
    bb_op_generate_jump(cursor.current);
    cursor.current->t = cp;               // jump back to continue -- check cond again
    cursor.current->f = bp;

    cursor_ingest();

    cursor.cp = NULL;
    cursor.bp = NULL;

    // cursor.current is NULL
    // return break 
    return bp;
}

// recursively generate IR from AST nodes 
// generates and returns arguments, and appends ops to the block
// Null-terminates -- last 'arg' should be a null arg
struct bb_arg *bb_gen_ir(ast_node *n)
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
            {
                struct bb_arg *a = bb_gen_ir(n->list.value);
                //if(a == NULL)
                //    goto error;

                if(n->list.next != NULL)
                {
                    bb_gen_ir(n->list.next);
                }
                return a;
            }
            break;
        case COMPOUND_SCOPE: // scope promotion -- this is stored as a "meta"
                             // ast node
            ENTER_SCOPE(n->cs.st);
            break;
        case IF:
            {
                cursor.current = bb_gen_if(n, cursor.current);
                break;
            }
        case WHILE:
            {
                cursor.current = bb_gen_while(n, cursor.current);
                break;
            }
        case RETURN:
            {
                struct bb_arg *a = bb_gen_ir(n->return_s.ret_expr);

                return bb_op_generate_return(a, cursor.current);
                break;
            }
        case BREAK:
            {
                if(cursor.bp == NULL)
                {
                    STDERR("Cannot use a break outside of a loop!");
                    return NULL;
                }
                // inject a jump to bp
                bb_op_generate_jump(cursor.current);
                cursor.current->t = cursor.bp;
                cursor_ingest();
                cursor.current = bb_create(&cursor);
            }
            break;
        case CONTINUE:
            {
                if(cursor.cp == NULL)
                {
                    STDERR("Cannot use a break outside of a loop!");
                    return NULL;
                }
                // inject a jump to cp
                bb_op_generate_jump(cursor.current);
                cursor.current->t = cursor.cp;
                cursor_ingest();
                cursor.current = bb_create(&cursor);            
            }
            break;
        case UNAOP:
            {
                switch(n->unaop.token)
                {
                    case '*':
                        {
                            struct bb_arg *a = bb_gen_ir(n->unaop.expression);
                            if(a == NULL)
                                goto error;
                            if(LEFTOP)
                                return a;
                            return bb_op_generate_load(
                                        a, create_arg(A_REG, NULL), cursor.current
                                    );
                        }
                        break;
                    case '&':
                        {
                            struct bb_arg *a = bb_gen_ir(n->unaop.expression);
                            if(a == NULL)
                                goto error;
                            if(LEFTOP)
                                return a;
                            return bb_op_generate_mov(
                                        a, create_arg(A_REG, NULL), cursor.current
                                    );
                        }
                        break;
//                    case SIZEOF:  // there is a crazy stack corruption going
//                                  // on.... unaop.expression becomes the ident
//                                  // code itself, causing immediate segfault.
//                                  // no clue what happened. obv there is no
//                                  // time left, so sizeof has to go
//                                  // the issues appear to occur within dim
//                                  // arrays, eg a[2]
//                        astprint(n->unaop.expression);
//                        return bb_op_generate_intconst(
//                                    calculate_sizeof(n->unaop.expression), cursor.current
//                                );
                    case PLUSPLUS:
                        {
                            struct bb_arg *a = bb_gen_ir(n->unaop.expression);
                            if(a == NULL)
                                goto error;
                        
                            // move a to dest
                            struct bb_arg *dest;
                            if(LEFTOP)
                                dest = a;
                            else
                            {
                                dest = bb_op_generate_mov(
                                        a, create_arg(A_REG, NULL), cursor.current
                                        );
                            }
                            // generate addition
                            struct bb_arg *one = bb_op_generate_intconst(1, cursor.current);
                            bb_op_generate_addition(
                                    a, one, cursor.current
                                    );
                            return dest;
                        }
                        break;
                    case MINUSMINUS:
                        {    
                            struct bb_arg *a = bb_gen_ir(n->unaop.expression);
                            if(a == NULL)
                                goto error;
                            // move a to dest
                            struct bb_arg *dest;
                            if(LEFTOP)
                                dest = a;
                            else
                            {
                                dest = bb_op_generate_mov(
                                        a, create_arg(A_REG, NULL), cursor.current
                                        );
                            }
                            // generate sub
                            struct bb_arg *one = bb_op_generate_intconst(1, cursor.current);
                            bb_op_generate_sub(
                                    a, one, cursor.current
                                    );
                            return dest;
                        }
                        break;
                    case '-':
                        struct bb_arg *a = bb_gen_ir(n->unaop.expression);
                        if(a == NULL)
                            goto error;
                        return bb_op_generate_neg(
                                    a, create_arg(A_REG, NULL), cursor.current
                                );
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
                            if(r == NULL || l == NULL)
                                goto error;
//                            // do i even need this ... . .. . . 
//                            if( (LITERALTYPE(l->am)) != (LITERALTYPE(r->am)) )
//                            {
//                                STDERR("Incompatible assignment!");
//                            }
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
                    case LOGAND:
                        //{

                        //    //cmp, br** produced
                        //    //set up jump
                        //    
                        //    struct bb *inter = bb_create(&cursor);
                        //    struct bb *f = bb_create(&cursor);
                        //    struct bb_arg *l = bb_gen_ir(n->binop.left);
                        //    cursor.current->t = inter;
                        //    cursor.current->f = f;
                        //    cursor_ingest();
                        //    // set current to fail
                        //    // this immediately jumps to inter's assigned fail
                        //    cursor.current = f;
                        //    bb_op_generate_jump(cursor.current);
                        //    cursor.current->t = inter->f;
                        //    cursor_ingest();

                        //    cursor.current = inter;
                        //    return bb_gen_ir(n->binop.right);
                        //}
                        //break;
                    case LOGOR:    
                        STDERR("&& / || not implemented yet");
                        break;
                    case '<':
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;
                            
                            bb_op_generate_cmp(l, r, cursor.current);
                            return bb_op_generate_brlt(cursor.current);
                        }
                    case '>':
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;
                            
                            bb_op_generate_cmp(l, r, cursor.current);
                            return bb_op_generate_brgt(cursor.current);
                        }
                    case LTEQ:
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;
                           
                            // conditional inversion
                            bb_op_generate_cmp(r, l, cursor.current);
                            return bb_op_generate_brgt(cursor.current);
                        }
                    case GTEQ:
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;
                            
                            // conditional inversion
                            bb_op_generate_cmp(r, l, cursor.current);
                            return bb_op_generate_brlt(cursor.current);
                        }
                    case EQEQ:
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;
                            
                            bb_op_generate_cmp(l, r, cursor.current);
                            return bb_op_generate_breq(cursor.current);
                        }
                    case NOTEQ:
                        {
                            struct bb_arg *r = bb_gen_ir(n->binop.right);
                            struct bb_arg *l = bb_gen_ir(n->binop.left);
                            if(r == NULL || l == NULL)
                                goto error;
                            
                            bb_op_generate_cmp(l, r, cursor.current);
                            return bb_op_generate_brneq(cursor.current);
                        }

                    default:
                        STDERR_F("Unhandled op \'%c\' when generating binop basic block", 
                                n->binop.token);
                        goto error;
                }
            }
            break;
        case TERNOP:
            STDERR("Ternary operators not supported!");
            goto error;
            break;
        // each expression is handled in place
        case FUNCTION:
            {
                struct bb_arg *l = bb_gen_ir(n->fncall.label);
                if(l == NULL)
                    goto error;
                ast_node *arghead = n->fncall.arglist;
                int32_t arg = 0;
                while(arghead != NULL)
                {
                    bb_op_generate_arg(arghead->list.value, arg, cursor.current);
                    arghead = arghead->list.next;
                    arg++;
                }

                return bb_op_generate_call(l, arg, cursor.current);
            }
            break;
        case NUMBER:
            return bb_op_generate_constant(n, cursor.current);
            break;
        case IDENT:
            {
                symtab_elem *si = symtab_lookup(current, n->ident.value, NS_IDENTS, -1);
                if(si == NULL)
                {
                    STDERR_F("FATAL: Couldnt find %s in symtab!", n->ident.value);
                    goto error;
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
