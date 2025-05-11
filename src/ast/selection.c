#include <stddef.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <parser/op.h>
#include <symtab/symtab.h>

ast_node *ast_create_if_stmt(
        ast_node *expression, ast_node *stmt, ast_node *else_stmt)
{
    ast_node *n = create_node(IF);

    n->if_s.expr = expression;
    n->if_s.stmt = stmt;
    n->if_s.else_stmt = else_stmt;

    return n;
}

ast_node *ast_create_switch_stmt(ast_node *expression, ast_node *stmt)
{
    ast_node *n = create_node(SWITCH);

    n->switch_s.expr = expression;
    n->switch_s.stmt = stmt;

    return n;
}

ast_node *ast_create_while_stmt(ast_node *expression, ast_node *stmt)
{
    ast_node *n = create_node(WHILE);

    n->while_s.expr = expression;
    n->while_s.stmt = stmt;

    return n;
}

ast_node *ast_create_do_while_stmt(ast_node *expression, ast_node *stmt)
{
    ast_node *n = create_node(DO);

    n->do_while_s.expr = expression;
    n->do_while_s.stmt = stmt;

    return n;
}

ast_node *ast_create_for_loop(
        ast_node *cl1, ast_node *cl2, ast_node *cl3, ast_node *stmt)
{
    ast_node *n = create_node(FOR);

    n->for_s.cl1 = cl1;
    n->for_s.cl2 = cl2;
    n->for_s.cl3 = cl3;

    n->for_s.stmt = stmt;

    return n;
}

ast_node *ast_create_goto(ast_node *ident)
{
    ast_node *n = create_node(GOTO);

    if(ident == NULL || ident->op_type != IDENT)
    {
        STDERR("GOTO statement was given an invalid ident!");
        return NULL;
    }

    n->goto_s.ident = ident;

    return n;
}

ast_node *ast_create_return(ast_node *ret_expression)
{
    ast_node *n = create_node(RETURN);

    n->return_s.ret_expr = ret_expression;

    return n;
}

ast_node *ast_create_label(ast_node *i, ast_node *stmt)
{
    ast_node *n = create_node(LABEL);

    n->label_s.expr = NULL;
    n->label_s.ident = i;
    n->label_s.stmt = stmt;
    
    return n;
}

ast_node *ast_create_case_label(ast_node *expr, ast_node *stmt)
{
    ast_node *n = create_node(CASE);

    n->label_s.expr = expr;
    n->label_s.ident = NULL;
    n->label_s.stmt = stmt;
    
    return n;
}

ast_node *ast_create_default_label(ast_node *stmt)
{
    ast_node *n = create_node(DEFAULT);

    n->label_s.expr = NULL;
    n->label_s.ident = NULL;
    n->label_s.stmt = stmt;

    return n;
}
