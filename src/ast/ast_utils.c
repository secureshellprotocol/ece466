#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>

#include <lexer/lexer.lex.h>

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

// semi broken
uint32_t calculate_sizeof(ast_node *spec)
{
    uint32_t sum = 0;

    if(spec && spec->op_type == LIST)
        return calculate_sizeof(spec->list.value);

    while(spec != NULL)
    {
        switch(spec->op_type)
        {  
            case INT:
                sum += sizeof(int);
                break;
            case LONG:
                sum += sizeof(long);
                break;
            case SHORT:
                sum += sizeof(short);
                break;
            case CHAR:
                sum += sizeof(char);
                break;
            case IDENT:
                astprint(spec->list.value);
                break;
            default:
                STDERR("Skipping:")
                astprint(spec->list.value);
                sum += sizeof(int); // assumption
                break;
        }
        spec = spec->list.next;
    }
    return sum;
}
