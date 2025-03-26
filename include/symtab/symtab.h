#ifndef __SYMTAB_H_JR
#define __SYMTAB_H_JR

#include <ast/ast.h>

/* Symbol table is just a null-terminated linked-list of elements.
 * to create a symtab, you create it's scope, and it contains 3 lists, each 
 * set to NULL.
 * Then, as you find stuff, you add it to the list. Must be easy
 */

enum {
    NS_LABELS,
    NS_SUE,
    NS_IDENTS
};

typedef struct symtab_elem_t {
    ast_node *n;
    char *file_origin;
    unsigned int line_no_origin;

    struct symtab_elem_t *next;
} symtab_elem;

typedef struct symbol_scope_t {
    struct symbol_scope_t *previous;

    symtab_elem *identifiers;    //idents ("everything else")
    symtab_elem *label_names;    //labels
    symtab_elem *sue_tags;       //struct, union, enum tags - members are
                                 //     implicit
} symbol_scope;

symbol_scope *symtab_create();

void symtab_destroy(symbol_scope *);

symtab_elem *symtab_lookup(symbol_scope *, char *name, int ns);

int symtab_enter(symbol_scope *, char *name, int ns, attr_list *l);

#endif
