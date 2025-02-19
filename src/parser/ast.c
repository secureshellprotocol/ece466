#include "src/lexer/lexer.lex.h"
#include "src/parser/ast.h"

// macro for `print_from_node(ast_node *)`
//  inserts a number of spaces equal to the depth of the tree.
//  think of justifying your indents during grade school -- good times!
#define JUSTIFY \
    printf("[%*s]", depth, ' ');

ast_node *create_node(int ot)
{
    ast_node *n = (ast_node *) calloc(1, sizeof(ast_node));
    
    n->op_type = ot;
    
    return n;
}   // free after use

// selecting some root node, start to print
void print_from_node(ast_node *n)
{
    static depth;

    JUSTIFY
            
    depth--;
    return;
}
