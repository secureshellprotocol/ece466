#include <stdlib.h>

#include <james_utils.h>
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

    if(list_node == NULL)
    {
        STDERR("NULL List provided! assuming you meant to start a list.");
        free(next);
        return ast_list_start(value);
    }

    next->list.value = value;
    // makes our value node into the head
    next->list.prev = list_node->list.prev;
    next->list.next = list_node;
    list_node->list.prev = next;

    return next;    // returns new head
}

// merge all items from donor list into list_node
ast_node *ast_list_merge(ast_node *list_node, ast_node *list_donor)
{
    ast_node *iter = list_node;
    while(iter->list.next != NULL)
        iter = iter->list.next;

    iter->list.next = list_donor;
    list_donor->list.prev = iter;
    
    return list_node;    // returns new head
}

ast_node *ast_list_reverse(ast_node *list_node)
{
    ast_node *tmp = list_node->list.next;
    list_node->list.next = list_node->list.prev;
    list_node->list.prev = tmp;

    if(list_node->list.prev == NULL)
        return list_node;
    return ast_list_reverse(list_node->list.prev);
}

