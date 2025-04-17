#include <stdio.h>
#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>


int verify_decl_specs(ast_node *decl_specs)
{
    #define ITER_ON_LIST() (decl_specs = decl_specs->list.next);
    
    static uint32_t specmask;
    
    specmask = 0;

    if(decl_specs == NULL) 
        return 1; // cant have an empty list
    
    while(decl_specs != NULL)
    {
        switch(decl_specs->list.value->op_type)
        {
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
            default:
                STDERR_F("Unrecognized op type %d", 
                        decl_specs->list.value->op_type);
                goto error;
        }
        ITER_ON_LIST(); 
    }
    return 0;

error:

    return 1;
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

ast_node *ast_create_decl(ast_node *decl_specs, ast_node *decl_list)
{
    ast_node *n = create_node(DECLARATION);

    n->d.decl_list = decl_list;
    n->d.decl_specs = decl_specs;
    n->d.stgclass = NULL;   //fine for now, should replicate symtab soon.
                            //whatever

    return n;
}

ast_node *ast_create_fndef(ast_node *decl_specs, ast_node *decl_list, 
        ast_node *stmt_list)
{
    ast_node *n = create_node(FNDEF);
    
    n->fndef.decl_list = decl_list;
    n->fndef.decl_specs = decl_specs; // same as return value
    n->fndef.stmt_list = stmt_list;
    
    return n;
}
