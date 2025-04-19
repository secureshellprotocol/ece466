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

ast_node *ast_create_func_call(ast_node *label, ast_node *arglist)
{
    ast_node *n = create_node(FUNCTION);
    
    if(label->op_type != LIST)
        label = ast_list_start(label);
    
    n->fncall.label = label;
    label->list.prev = n;

    n->fncall.arglist = arglist;
    return n;
}

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

ast_node *ast_create_fndef_decl(ast_node *decl, ast_node *stmt_list)
{
    ast_node *n = create_node(FNDEF);
   
    n->fndef.decl = decl;
    n->fndef.stmt_list = stmt_list;
    
    return n;
}
