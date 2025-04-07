#ifndef __SYMTAB_H_JR
#define __SYMTAB_H_JR

#include <ast/ast.h>

/* Symbol table is just a null-terminated linked-list of elements.
 * to create a symtab, you create it's scope, and it contains 3 lists, each 
 * set to NULL.
 * Then, as you find stuff, you add it to the list. Must be easy
 */

enum namespaces {
    NS_LABELS,
    NS_SUE,
    NS_IDENTS,
    NS_MEMBERS
};

typedef struct symtab_elem_t {
    ast_node *n;
    char *name;

    char *file_origin;
    unsigned int line_no_origin;

    struct symtab_elem_t *next;
} symtab_elem;

typedef struct symbol_scope_t {
    struct symbol_scope_t *previous;

    symtab_elem *idents;    //idents ("everything else")
    symtab_elem *labels;    //labels
    symtab_elem *sue_tags;       //struct, union, enum tags - members are
                                 //                           implicit
} symbol_scope;

// creates a symbol table scope -- must supply a pointer to a previous scope, or
// NULL if this is the root/file scope.
symbol_scope *symtab_create(symbol_scope *);

// destroys a symtab and all assoc. namespaces
// preserves previous namespace
void symtab_destroy(symbol_scope *);

// linearly searches for a named symbol in a specified namespace
//  if not found, returns NULL.
symtab_elem *symtab_lookup(symbol_scope *, char *name, int ns);

// appends a symbol into a namespace. can provide optional attribute list
// returns -1 on failure
// returns 0 on success
int symtab_enter(symbol_scope *, char *name, int ns, ast_node *d,
        char *file_origin, unsigned int line_no_origin);

void symtab_install(symbol_scope *scope, ast_node *n);

void symtab_install_decl(symbol_scope *scope, ast_node *d);

#endif
