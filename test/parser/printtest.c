#include <string.h>

#include <lexer/tokens.h>
#include <lexer/lex_utils.h>
#include <parser/ast.h>

// recreate 
//      xyz=(1024+abc);
int main()
{
    static char buf[16];
    ast_node *root = create_node((int) '=');
    
    root->binop.left = create_node(IDENT);
    root->binop.left->ident.value = strncpy(buf, "xyz", 16);

    root->binop.right = create_node((int) '+');
    root->binop.right->binop.left = create_node(NUMBER);
    root->binop.right->binop.left->num.ival = 1024;
    root->binop.right->binop.left->num.tags = 0;
    root->binop.right->binop.right = create_node(IDENT);
    root->binop.right->binop.right->ident.value = strncpy(buf, "abc", 16);
    
    print_from_node(root);        
    
    return 0;
}
