#include <stdio.h>
#include <stdlib.h>

#include <ast/ast.h>
#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>

// macro for `astprint(ast_node *)`
//  inserts a number of spaces equal to the depth of the tree.
//  think of justifying your indents during grade school -- good times!
#define JUSTIFY \
    printf("%*c", depth+1, ' ');

// selecting some root node, start to print
void astprint(ast_node *n)
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
    case CHARLIT:
        printf("CHARLIT ");
        chardecode(n->char_array.value[0]);
        printf("\n");
        break;
    case STRING:
        printf("STRING ");
        for(int i = 0; i < n->char_array.s_len; i++)
        {
            chardecode(n->char_array.value[i]);
        }
        printf("\n");
        break;
    case UNAOP:
        {
            char *tok_id = get_token_id(n->binop.token);
            printf("UNARY OP %s\n", tok_id); 
        }
        if(n->unaop.expression != NULL)
            astprint(n->unaop.expression);

        break;
    case BINOP:
        {
            char *tok_id = get_token_id(n->binop.token);
            printf("BINARY OP %s\n", tok_id); 
        }
        if(n->binop.left != NULL)
            astprint(n->binop.left);
        if(n->binop.right != NULL)
            astprint(n->binop.right);

        break;
    case TERNOP:
        printf("TERNARY OP \n"); 
        
        if(n->ternop.left != NULL)
            astprint(n->ternop.left);
         
        if(n->ternop.middle != NULL)
            astprint(n->ternop.middle);

        if(n->ternop.right != NULL)
            astprint(n->ternop.right);
        
        break;           
    }

    depth--;
    return;
}
