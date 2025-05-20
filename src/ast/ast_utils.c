#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>
#include <lexer/lexer.lex.h>
#include <symtab/symtab.h>

extern symbol_scope *current;

// not async safe
// these macros are defined in
//  include/lexer/lexer.lex.h
    
// verify_decl_specs roadmap
//  Qualifier verification stage
//      record whether we are CONST or VOLATILE or RESTRICT or NOTHING
//      make sure we are only one of these
//
//  Specifier verification stage
//  layer 1
//      record whether we are SIGNED or UNSIGNED or NOTHING.
//  layer 2
//      record whether we are LONG or LONGLONG or SHORT or NOTHING.
//  layer 3
//      record whether we are CHAR or INT or FLOAT or DOUBLE or VOID.
//
//          CHAR:
//              1: SIGNED or UNSIGNED or NOTHING
//              2: NOTHING
//          INT:
//              1: SIGNED or UNSIGNED or NOTHING
//              2: LONG or LONGLONG or SHORT or NOTHING
//          DOUBLE:
//              1: NOTHING
//              2: LONG OR NOTHING
//          FLOAT or VOID:
//              none at all!

//  some convenience macros are added here.

#define QUALIFIER_CONFLICT(specmask) \
    (IS_CONST(specmask) || IS_RESTRICT(specmask) || IS_VOLATILE(specmask))

#define LAYER_1_CONFLICT(specmask) \
    (IS_SIGNED(specmask) || IS_UNSIGNED(specmask))

// we do an additional check for LONGs to see if we need to be promoted to
// LONGLONG or not.
#define LAYER_2_CONFLICT(specmask) \
    (IS_LONGLONG(specmask) || IS_SHORT(specmask))

#define LAYER_3_CONFLICT(specmask)  \
    (IS_CHAR(specmask) || IS_INT(specmask) || IS_FLOAT(specmask) || IS_DOUBLE(specmask) || IS_VOID(specmask))

int verify_decl_specs(ast_node *decl_specs)
{
    #define ITER_ON_LIST() (decl_specs = decl_specs->list.next);
    
    static uint32_t qualmask;
    static uint32_t specmask;

    qualmask = 0;
    specmask = 0;

    if(decl_specs == NULL) 
        goto error; // cant have an empty list
 
    while(decl_specs != NULL)
    {    

        switch(decl_specs->list.value->op_type)
        {
            // qualifier verification
            case CONST:
                if(QUALIFIER_CONFLICT(qualmask))
                {
                    STDERR("Too many type qualifiers specified!");
                    goto error;
                }
                TAG_SET(qualmask, TQ_CONST);
                ITER_ON_LIST();
                break;
            case RESTRICT:
                if(QUALIFIER_CONFLICT(qualmask))
                {
                    STDERR("Too many type qualifiers specified!");
                    goto error;
                }
                TAG_SET(qualmask, TQ_RESTRICT);
                ITER_ON_LIST();
                break;
            case VOLATILE:
                if(QUALIFIER_CONFLICT(qualmask))
                {
                    STDERR("Too many type qualifiers specified!");
                    goto error;
                }
                TAG_SET(qualmask, TQ_VOLATILE);
                ITER_ON_LIST();
                break;
            // layer 1
            case SIGNED:
                if(LAYER_1_CONFLICT(specmask))
                {   // todo make more useful later
                    STDERR_F("Bad decl specs! specified signed with %u", specmask);
                    goto error;
                }
                if(IS_DOUBLE(specmask) || IS_FLOAT(specmask))
                {
                    STDERR("cannot have a signed real value!");
                    goto error;
                }
                TAG_SET(specmask, TS_SIGNED);
                ITER_ON_LIST();
                break;
            case UNSIGNED:
                if(LAYER_1_CONFLICT(specmask))
                {   // todo make more useful later
                    STDERR_F("Bad decl specs! specified unsigned with %u", specmask);
                    goto error;
                }
                if(IS_DOUBLE(specmask) || IS_FLOAT(specmask))
                {
                    STDERR("cannot have an unsigned real value!");
                    goto error;
                }
                TAG_SET(specmask, TS_UNSIGNED);
                ITER_ON_LIST();
                break;
            // layer 2
            case LONG:
                if(LAYER_2_CONFLICT(specmask))
                {
                    STDERR_F("Bad decl specs! specified long with %u", specmask);
                    goto error;
                }
                if(IS_LONG(specmask))
                {
                    TAG_SET(specmask, TS_LONGLONG);
                }
                else
                {
                    TAG_SET(specmask, TS_LONG);
                }
                if(IS_FLOAT(specmask))
                {
                    STDERR("cannot have a long double!");
                    goto error;
                }
                if(IS_CHAR(specmask))
                {
                    STDERR("cannot have a long char!");
                    goto error;
                }
                ITER_ON_LIST();
                break;
            case SHORT:
                if(LAYER_2_CONFLICT(specmask))
                {
                    STDERR_F("Bad decl specs! specified short with %u", specmask);
                    goto error;
                }
                TAG_SET(specmask, TS_SHORT);
                ITER_ON_LIST();
                break;
            // layer 3
            case CHAR:
                if(LAYER_3_CONFLICT(specmask))
                {
                    STDERR_F("Bad decl specs! specified char with %u", specmask);
                    goto error;
                }
                if(LAYER_2_CONFLICT(specmask))
                {
                    STDERR("cannot specify long or short with char!");
                    goto error;
                }
                TAG_SET(specmask, TS_CHAR);
                ITER_ON_LIST();
                break;
            case INT:
                if(LAYER_3_CONFLICT(specmask))
                {
                    STDERR_F("Bad decl specs! specified int with %u", specmask);
                }
                TAG_SET(specmask, TS_INT);
                ITER_ON_LIST();
                break;
            case FLOAT:
                if(LAYER_1_CONFLICT(specmask) || LAYER_2_CONFLICT(specmask) || LAYER_3_CONFLICT(specmask))
                {
                    STDERR("Cannot specify float with other specifiers!");
                    goto error;
                }
                TAG_SET(specmask, TS_FLOAT);
                ITER_ON_LIST();
                break;
            case DOUBLE:
                if(LAYER_3_CONFLICT(specmask))
                {
                    STDERR_F("Bad decl specs! specified double with %u", specmask);
                    goto error;
                }
                TAG_SET(specmask, TS_DOUBLE);
                ITER_ON_LIST();
                break;
            case VOID:
                if(LAYER_1_CONFLICT(specmask) || LAYER_2_CONFLICT(specmask) || LAYER_3_CONFLICT(specmask))
                {
                    STDERR("Cannot specify void with other specifiers!");
                    goto error;
                }
                TAG_SET(specmask, TS_VOID);
                ITER_ON_LIST();
                break;
            default:
                if((LAYER_1_CONFLICT(specmask) || LAYER_2_CONFLICT(specmask)) && !(LAYER_3_CONFLICT(specmask)))
                {
                    // default to int
                    TAG_SET(specmask, TS_INT);
                    break;
                }
                STDERR("How did we get here? have");
                astprint(decl_specs->list.value);
                break;
        }
    }
    
    return 0;

error:
    return -1;
}

uint32_t calculate_sizeof(ast_node *n)
{
    if(n == NULL)
    {
        STDERR("Provided NULL node!");
    }
    
    if(n->op_type == LIST)
        n = n->list.value;

    astprint(n);

    switch(n->op_type)
    {
        case IDENT:
            return ast_get_ident_size(n);
            break;
        case UNAOP:
            {
                if(n->unaop.token == '*')
                {
                    return calculate_sizeof(n->unaop.expression);
                }
            }
            break;
        case BINOP:
            {
                int l, r;
                l = calculate_sizeof(n->binop.left);
                r = calculate_sizeof(n->binop.right);
                if(l > r) 
                    return l;
                return r;
            }
            break;
        default:
            break;
    }
    return 0;
}

uint32_t ast_get_type_size(ast_node *d)
{
    uint32_t tags = 0;
    uint32_t sum = 0;

    if(d == NULL || d->d.decl_specs == NULL)
    {
        STDERR("Provided null specifiers when calculating sizeof!");
        return 0;
    }

    ast_node *spec = d->d.decl_specs;
    while(spec)
    {
        switch(spec->list.value->op_type)
        {  
            case INT:
                if(IS_INT(tags)) break;
                TAG_SET(tags, TS_INT);        
                sum = sizeof(int);
                break;
            case LONG:
                if(IS_LONG(tags))
                {
                    if(!IS_LONGLONG(tags))
                    {
                        TAG_SET(tags, TS_LONGLONG);
                        sum = sizeof(long long);
                    }
                    break;
                }
                TAG_SET(tags, TS_LONG);
                sum = sizeof(long);
                break;
            case SHORT:
                if(IS_SHORT(tags)) break;
                TAG_SET(tags, TS_SHORT);
                sum = sizeof(short);
                break;
            case CHAR:
                if(IS_CHAR(tags)) break;
                TAG_SET(tags, TS_CHAR);
                sum = sizeof(char);
                break;
            case VOID:
                if(IS_VOID(tags)) break;
                TAG_SET(tags, TS_VOID);
                sum = sizeof(void);
                break;
            default:
                STDERR("Skipping:") // signed, unsigned, float... we dont care
                //astprint(spec->list.value);
                break;
        }
        spec = spec->list.next;
    }
    
    // can be valid, eg "unsigned x;"
    if(tags == 0)
        sum = sizeof(int);

   
    ast_node *decls = d->d.declarator;
    while(decls)
    {
        switch(decls->list.value->op_type)
        {
            case POINTER:
                sum = sizeof(int *);
                break;
            case ARRAY:
                sum *= decls->list.value->array.size->num.ival;
                break;
            default:
                break;
        }
        decls = decls->list.next;
    }

    return sum;
}

uint32_t ast_get_ident_size(ast_node *ident)
{
    if(ident == NULL || ident->op_type != IDENT)
    {
        STDERR("Not an ident!");
        if(ident != NULL)
            astprint(ident);
        return 0;
    }

    symtab_elem *s = symtab_lookup(
            current,
            ident->ident.value,
            NS_IDENTS, -1
            );
    
    return ast_get_type_size(s->d);
}



