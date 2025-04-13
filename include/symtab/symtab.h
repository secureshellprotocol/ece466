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
    SCOPE_SUE
};

typedef struct symtab_elem_t symtab_elem;
typedef struct symbol_scope_t symbol_scope;

typedef struct symtab_elem_t {
    ast_node *n;    // var, func, sue node.
                    // this contains stgclass and attrs
    char *key;      // typically a ptr into n

    char *file_origin;
    unsigned int line_no_origin;

    struct symtab_elem_t *next; // next element in scope.
} symtab_elem;

typedef struct symbol_scope_t {
    struct symbol_scope_t *previous;
    uint32_t name;

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
symbol_scope *symtab_create(symbol_scope *p, uint32_t scope_name, 
        char *origin_file, uint32_t origin_lineno);

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
void symtab_install_list(symbol_scope *scope, ast_node *l);

// src/symtab/symtabprint.c

void symtabprint(char *ident, char *file_name, unsigned int line_num, 
        ast_node *attrs, symbol_scope *scope);

char *scopedecode(uint32_t scope);

char *nsdecode(int ns);
#endif
