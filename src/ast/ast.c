#include <stdio.h>
#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>

// not async safe
// these macros are defined in
//  include/lexer/lexer.lex.h
int verify_decl_specs(ast_node *decl_specs)
{
    #define ITER_ON_LIST() (decl_specs = decl_specs->list.next);
    
    static uint32_t specmask;
    static uint32_t qualmask;

    specmask = 0;
    qualmask = 0;

    if(decl_specs == NULL) 
        goto error; // cant have an empty list
    
    while(decl_specs != NULL)
    {
        switch(decl_specs->list.value->op_type)
        {
            // type specifiers
            case VOID:
                if(IS_VOID(specmask))
                {
                    STDERR("void already specified! bailing");
                    goto error;
                }
                TAG_SET(specmask, TS_VOID);
                break;
            case CHAR:
                if(IS_CHAR(specmask))
                {
                    STDERR("char already specified! bailing");
                    goto error;
                }
                TAG_SET(specmask, TS_CHAR);
                break;
            case SHORT:
                if(IS_SHORT(specmask))
                {
                    STDERR("short already specified! bailing");
                    goto error;
                }
                TAG_SET(specmask, TS_SHORT);
                break;
            case INT:
                if(IS_INT(specmask))
                {
                    STDERR("int already specified! bailing");
                    goto error;
                }
                TAG_SET(specmask, TS_INT);
                break;
            case LONG:
                if(IS_LONG(specmask))
                {
                    if(IS_LONGLONG(specmask))
                    {
                        STDERR("Too long of a long!")
                    }
                }
                break;
            case FLOAT:
                if(IS_FLOAT(specmask))
                {
                    STDERR("float already specified! bailing");
                    goto error;
                }
                TAG_SET(specmask, TS_FLOAT);
                break;
            case DOUBLE:
                if(IS_DOUBLE(specmask))
                {
                    STDERR("double already specified! bailing");
                    goto error;
                }
                TAG_SET(specmask, TS_DOUBLE);
                break;
            case SIGNED:
                if(IS_SIGNED(specmask))
                {
                    STDERR("signed already specified! bailing");
                    goto error;
                }
                TAG_SET(specmask, TS_SIGNED);
                break;
            case UNSIGNED:
                if(IS_UNSIGNED(specmask))
                {
                    STDERR("unsigned already specified! bailing");
                    goto error;
                }
                TAG_SET(specmask, TS_UNSIGNED);
                break;
            // type qualifiers -- can only have one!
            case CONST:
                if(specmask != 0)
                {
                    STDERR_F("Some qualifier is already set! %d", specmask);
                    goto error;
                }
                TAG_SET(specmask, TQ_CONST);
                break;
            case RESTRICT:
                if(specmask != 0)
                {
                    STDERR_F("Some qualifier is already set! %d", specmask);
                    goto error;
                }
                TAG_SET(specmask, TQ_RESTRICT);
                break;
            case VOLATILE:
                if(specmask != 0)
                {
                    STDERR_F("Some qualifier is already set! %d", specmask);
                    goto error;
                }
                TAG_SET(specmask, TQ_VOLATILE);
                break;
            default:
                STDERR_F("Unrecognized op type %d", 
                        decl_specs->list.value->op_type);
                goto error;
        }
        ITER_ON_LIST(); 
    }
    return 0;

error:

    return -1;
}

ast_node *create_node(int ot)
{
    ast_node *n = (ast_node *) calloc(1, sizeof(ast_node));
    n->op_type = ot;
    
    return n;
}

ast_node *ast_create_ident(struct yy_struct ys)
{
    ast_node *n = create_node(IDENT);
    n->ident.value = ys.s;
    return n;
}

ast_node *ast_create_num(struct yy_struct ys)
{
    ast_node *n = create_node(NUMBER);
    switch(IS_FLOAT(ys.tags))
    {
    case 0: /* integer */
        n->num.ival = ys.ulld;
        break;
    default: /* real */
        n->num.fval = ys.ldf;
        break;
    }
    n->num.tags = ys.tags;
    return n;
}

ast_node *ast_create_constant(unsigned long long int ulld)
{
    ast_node *n = create_node(NUMBER);
    n->num.ival = ulld;
    n->num.tags = U_BIT;
    return n;
}

ast_node *ast_create_string(struct yy_struct ys)
{
    ast_node *n = create_node(STRING);
    n->char_array.value = ys.s;
    n->char_array.s_len = ys.s_len;
    return n;
}

ast_node *ast_create_charlit(struct yy_struct ys)
{
    ast_node *n = create_node(CHARLIT);
    n->char_array.value = ys.s;
    n->char_array.s_len = 1;
    return n;
}

ast_node *ast_create_unaop(int token, ast_node *e)
{
    ast_node *n = create_node(UNAOP);
    n->unaop.token = token;
    n->unaop.expression = e;
    
    return n;
}

ast_node *ast_create_binop(int token, ast_node *l, ast_node *r)
{
    ast_node *n = create_node(BINOP);
    n->binop.token = token;

    n->binop.left = l;
    n->binop.right = r;
    return n;
}

ast_node *ast_create_ternop(
        ast_node *l, ast_node *m, ast_node *r)
{
    ast_node *n = create_node(TERNOP);

    n->ternop.left = l;
    n->ternop.middle = m;
    n->ternop.right = r;
    return n;
}

ast_node *ast_create_func_call(ast_node *label, ast_node *arglist)
{
    ast_node *n = create_node(FUNCTION);
    
    n->fncall.label = label;
    n->fncall.arglist = arglist;
    return n;
}

//ast_node *ast_create_decl(ast_node *decl_specs, ast_node *decl_list)
//{
//    if(decl_list == NULL || decl_specs == NULL)
//    {
//        STDERR("Recieved invalid arguments!")
//        return NULL;
//    }
//
//    ast_node *n = create_node(DECLARATION);
//   
//    n->var.i = decl_list->list.value;
//    n->var.stgclass = NULL;
//    n->var.attr_list = decl_list->list.next;
//    
//    ast_node *current_node = n->var.attr_list;
//
//    while(current_node != NULL)
//    {
//        switch(current_node->list.value->op_type)
//        {
//            case TYPEDEF: case EXTERN: case STATIC: case AUTO: case REGISTER:
//                if(current_node->list.next != NULL)
//                {
//                    STDERR_F("Multiple storage classes specified for %s! bailing",
//                            n->var.i->ident.value);
//                    return NULL;
//                }
//                
//                n->var.stgclass = current_node->list.value; // decouple storage
//                                                            // class from list
//                current_node->list.prev->list.next = NULL; 
//                break;
//            default:
//                break;
//        }
//        current_node = current_node->list.next;
//    }
//
//    if(verify_attr_list(n->var.attr_list) == 1)
//    {
//        STDERR_F("Failed to create %s!", n->var.i->ident.value);
//        return NULL;
//    }
//    return n;
//}

ast_node *ast_create_var_decl(ast_node *decl_specs, ast_node *decl_list)
{
    if(decl_specs == NULL)
    {
        STDERR("No declaration specifiers supplied!");
        goto error;
    }
    if(decl_list == NULL)
    {
        STDERR("No declarators supplied!")
        return NULL;
    }
    
    ast_node *stgclass = NULL;
    
    while(1)
    {
        switch(decl_specs->list.value->op_type)
        {
            case TYPEDEF: case EXTERN: case STATIC: case AUTO: case REGISTER:
                if(stgclass != NULL)
                {
                    STDERR("Multiple storage classes specified! found");
                    astprint(stgclass);
                    astprint(decl_specs->list.value);
                    return NULL;
                }
                stgclass = decl_specs->list.value;
                decl_specs = decl_specs->list.next;

                decl_specs->list.prev = NULL;
                break;
            default:
                goto stgclass_done;
        }
    }
stgclass_done:  // its possible for stgclass to be NULL -- thats caught in the
                // symtab installation
    

    if(verify_decl_specs(decl_specs) == -1)
    {
        STDERR("Invalid declaration specifiers supplied!");
        goto error;
    }

    /*      topology of man and society
     *
     *  extern int x, *y;
     *  
     *  |-[extern]
     *  |
     *  | --[int]
     *  |
     *  \ --[decl_list] ---- [decl_list]
     *      |               |
     *      [ident x]       [ptr]
     *                      |
     *                      [ident y]
     *
     * becomes
     *  
     *  | -[decl_x]-\
     *  |           |-[extern]      // stgclass
     *  |           |-[int]         // decl_specs
     *  |           |-  \
     *  |               | - [ident x]     // declarator
     *  |
     *  \ -[decl_y]-\
     *              |-[extern]
     *              |-[int]
     *              |-  \
     *                  | - [ptr]-[ident y]
     */
    
    // according to society, we iterate over the list and derive our final decls
    // in the end -- this will probably get relegated to a specific "decl" ast
    // node. however im lazy and threw the bare min into that. yet i have time
    // for graphics.

    ast_node *final_decl_list = NULL;
    while(decl_list != NULL)
    {
        ast_node *decl = create_node(DECLARATION);
        decl->d.stgclass = stgclass;
        
        decl->d.decl_specs = decl_specs;
        decl->d.declarator = decl_list->list.value; // also a list.
    
        if(final_decl_list == NULL)
        {
            final_decl_list = ast_list_start(decl);
        }
        else
        {
            final_decl_list = ast_list_insert(final_decl_list, decl);
        }

        decl_list = decl_list->list.next;
    }

    return final_decl_list;

error:
    return NULL;
}

//ast_node *ast_create_fndef_decl(ast_node *decl_specs, ast_node *decl_list, 
//        ast_node *stmt_list)
//{
//    ast_node *n = create_node(FNDEF);
//    
//    //n->fndef.decl_list = decl;
//    n->fndef.decl_specs = decl_specs; // same as return value
//    n->fndef.stmt_list = stmt_list;
//    
//    return n;
//}
