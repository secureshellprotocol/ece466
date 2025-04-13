#include <stdio.h>
#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>

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

ast_node *ast_create_func(ast_node *label, ast_node *decl_specs, ast_node *params_list)
{
    ast_node *n = create_node(FUNCTION);
    
    n->func.label = label;
    n->func.decl_specs = decl_specs;
    n->func.params_list = params_list;
    return n;
}

#define ITER_ON_LIST() (decl_list = decl_list->list.next);
ast_node *ast_create_var(ast_node *decl_list)
{
    if(decl_list == NULL)
    {
        return NULL;
    }

    ast_node *n = create_node(VARIABLE);
    
    n->var.i = decl_list->list.value;
    
    if(decl_list->list.next == NULL)
    {
        STDERR_F("symtab_install: No declspecs specified for %s!",
                decl_list->list.value);
        return NULL;
    }
    
    ITER_ON_LIST();
    // determine storage class
    switch(decl_list->list.value->op_type)
    {
        case TYPEDEF:
            STDERR("symtab_install: TYPEDEF is not supported! Throwing this decl away.");
            return NULL;
        case EXTERN: case STATIC: case AUTO: case REGISTER:
            n->var.stgclass = decl_list->list.next->list.value;
            ITER_ON_LIST();
            break;
        default:
            // symtab will detect this, and choose the right stgclass
            n->var.stgclass = NULL;
            break;
    }

    if(verify_attr_list(decl_list) == 1)
    {
        STDERR_F("ast_create_var: Failed to create %s!", n->var.i);
        return NULL;
    }

    return n;
}

int verify_attr_list(ast_node *decl_list)
{
    static uint32_t specmask;
    if(decl_list == NULL) return 1; // cant have an empty list
    while(decl_list != NULL)
    {
        switch(decl_list->list.value->op_type)
        {
            case VOID:
                if(IS_VOID(specmask))
                {
                    STDERR("verify_attr_list: void already specified! bailing");
                    goto error;
                }
                TAG_SET(specmask, TS_VOID);
                break;
            case CHAR:
            case SHORT:
            case INT:
            case LONG:
                if(IS_LONG(specmask))
                {
                    if(IS_LONGLONG(specmask))
                    {
                        stderr()
                    }
                }
            case FLOAT:
            case DOUBLE:
            case SIGNED:
            case UNSIGNED:
            case BOOL:
                break;
            default:
                goto error;
        }
    }
    return 0;

error:
    return 1;
}

