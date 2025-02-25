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
}   // free after use

// walks up node tree and deletes each node
void free_node(ast_node *n)
{
    if(n == NULL) { return; }

    switch(n->op_type)
    {
    case IDENT:
        goto freedom;
    case NUMBER:
        goto freedom;
    case UNAOP:
        free_node(n->unaop.expression);
        goto freedom;
    case BINOP:
        free_node(n->binop.left);
        free_node(n->binop.right);
        goto freedom;
    case TERNOP:
        free_node(n->ternop.left);
        free_node(n->ternop.middle);
        free_node(n->ternop.right);
        goto freedom;
    }

freedom:
    free(n);
    return;
}
