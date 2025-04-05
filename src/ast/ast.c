#include <stdio.h>
#include <stdlib.h>

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

ast_node *ast_create_decl(ast_node *decl_specs, ast_node *decl_list)
{
    ast_node *n = create_node(DECLARATION);
    n->decl.decl_specs = decl_specs;
    n->decl.decl_list = decl_list;

    return n;
}

// walks up node tree and deletes each node
// NO PROTECTION -- ADD A REFERENCE COUNT QUESTION MARK????
// Hak -- i know this isnt required, but i wanted to try anyways
void free_node(ast_node *n)
{
    if(n == NULL) { return; }

    switch(n->op_type)
    {
    case UNAOP:
        free_node(n->unaop.expression);
        break;
    case BINOP:
        free_node(n->binop.left);
        free_node(n->binop.right);
        break;
    case TERNOP:
        free_node(n->ternop.left);
        free_node(n->ternop.middle);
        free_node(n->ternop.right);
        break;
    default:
        break;
    }

    free(n);
    return;
}

