#include <stdlib.h>

#include <ast/ast.h>
#include <parser/op.h>

ast_node *ast_list_start(ast_node *start)
{
    ast_node *n = create_node(LIST);
    n->list.value = start;
    n->list.prev = NULL;
    n->list.next = NULL;

    return n;
}

// insert a new list node before the current list_node element.
ast_node *ast_list_insert(ast_node *list_node, ast_node *value)
{
    ast_node *next = create_node(LIST);
    next->list.value = value;
   
    next->list.prev = list_node->list.prev;
    next->list.next = list_node;
    list_node->list.prev = next;

    return next;
}
