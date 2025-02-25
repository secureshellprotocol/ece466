#include <stdio.h>
#include <stdlib.h>

#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>
#include <parser/ast.h>
#include <parser/op.h>

// macro for `print_from_node(ast_node *)`
//  inserts a number of spaces equal to the depth of the tree.
//  think of justifying your indents during grade school -- good times!
#define JUSTIFY \
    printf("%*c", depth+1, ' ');

ast_node *create_node(int ot)
{
    ast_node *n = (ast_node *) calloc(1, sizeof(ast_node));
    
    n->op_type = ot;
    
    return n;
}   // free after use

// selecting some root node, start to print
void print_from_node(ast_node *n)
{
    static int depth;
    ++depth;

    JUSTIFY
    switch(n->op_type)
    {
    case IDENT:
        printf("IDENT %s\n", n->ident.value);
        break;
    case NUMBER:
        printf("NUM (numtype=");
        if(IS_FLOATING(n->num.tags))
            printf("float) %Lg\n", n->num.fval);
        else printf("int) %llu\n", n->num.ival);
        break;
    case UNAOP:
        {
            char *tok_id = get_token_id(n->binop.token);
            printf("UNARY OP %s\n", tok_id); 
        }
        if(n->unaop.expression != NULL)
            print_from_node(n->unaop.expression);

        break;
    case BINOP:
        {
            char *tok_id = get_token_id(n->binop.token);
            printf("BINARY OP %s\n", tok_id); 
        }
        if(n->binop.left != NULL)
            print_from_node(n->binop.left);
        if(n->binop.right != NULL)
            print_from_node(n->binop.right);

        break;
    case TERNOP:
        printf("TERNARY OP \n"); 
        
        if(n->ternop.left != NULL)
            print_from_node(n->ternop.left);
         
        if(n->ternop.middle != NULL)
            print_from_node(n->ternop.middle);

        if(n->ternop.right != NULL)
            print_from_node(n->ternop.right);
        
        break;           
    }

    depth--;
    return;
}
