#ifndef __SYMTAB_H_JR
#define __SYMTAB_H_JR

#include <stdint.h>

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

enum scopes {
    SCOPE_GLOBAL,
    SCOPE_FUNCTION,
    SCOPE_BLOCK,
    SCOPE_SUE
};

typedef struct symtab_elem_t symtab_elem;
typedef struct symbol_scope_t symbol_scope;

typedef struct symtab_elem_t {
//    ast_node *declarator;
//    ast_node *decl_specs;
//    ast_node *stgclass;
    ast_node *d;

    char *key;
    int complete;

    char *file_origin;
    unsigned int line_num_origin;

    struct symtab_elem_t *next; // next element in scope.
} symtab_elem;

typedef struct symbol_scope_t {
    struct symbol_scope_t *previous;
    //char *name;
    enum scopes scope;

    char *origin_file;      // where the scope starts
    uint32_t origin_lineno; // line where scope starts

    symtab_elem *idents;        //idents ("everything else")
    symtab_elem *labels;        //labels
    symtab_elem *sue_tags;       //struct, union, enum tags - members are
                                 //                           implicit
} symbol_scope;

// src/symtab/symtab.c

// creates a symbol table scope -- must supply a pointer to a previous scope, or
// NULL if this is the root/file scope.
symbol_scope *symtab_create(symbol_scope *p, enum scopes scope_name, 
        char *origin_file, uint32_t origin_lineno);

// destroys a symtab and all assoc. namespaces
// preserves and returns previous namespace (possibly null)
symbol_scope *symtab_destroy(symbol_scope *);

// linearly searches for a named symbol in a specified namespace
//  if not found, returns NULL.
symtab_elem *symtab_lookup(symbol_scope *, char *name, int ns);

// appends a symbol into a namespace. can provide optional attribute list
// returns -1 on failure
// returns 0 on success
int symtab_enter(symbol_scope *, char *name, enum namespaces ns, 
        ast_node *v, char *file_origin, unsigned int line_no_origin);
int _symtab_inject_elem(symbol_scope *scope, enum namespaces ns, symtab_elem *e);

// symbol table insertion front-end: just needs a variable node and scope
void symtab_install(symbol_scope *scope, ast_node *decl_list, 
        char *yyin_name, unsigned int line_num);

// src/symtab/symtabprint.c

void symtabprint(symbol_scope *scope, enum namespaces ns, char *l);

char *stgclassdecode(ast_node *n);
char *scopedecode(enum scopes scope);
char *nsdecode(enum namespaces ns);
char *nodetypedecode(ast_node *n);

#endif
