#include <string.h>

#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>
#include <parser/ast.h>
#include <parser/op.h>

// recreate 
//      xyz=(1024+abc);
int main()
{
    ast_node *root = create_node(BINOP);
    
    root->binop.token = (int) '=';
    root->binop.left = create_node(IDENT);
    root->binop.left->ident.value = strdup("xyz");

    root->binop.right = create_node(BINOP);
    root->binop.right->binop.token = (int) '+';
    root->binop.right->binop.left = create_node(NUMBER);
    root->binop.right->binop.left->num.ival = 1024;
    root->binop.right->binop.left->num.tags = 0;
    root->binop.right->binop.right = create_node(IDENT);
    root->binop.right->binop.right->ident.value = strdup("abc");
    
    print_from_node(root);        
    
    return 0;
}
