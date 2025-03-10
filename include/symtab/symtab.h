typedef struct symtab_element_t {
    char *key;
    int occupied;

    ast_node *n;
} symtab_element;

typedef struct symtab_t {
    unsigned int filled;    // num of slots filled in elements array
    unsigned int max_cap;   // total capacity of elements array

    ast_node **elems;
} symtab;

typedef struct symbol_scope_t {
    struct symbol_scope_t *previous;

    symtab *label_names;    //labels
    symtab *sue_tags;       //struct, union, enum tags
    symtab *identifiers;    //idents ("everything else")
} symbol_scope

// Returns a valid pointer to a symbol table, or a null pointer if it failed to
// allocate
symtab *t symtab_create()
{

}

// Insert node into symbol table
// If needed, perform in-place rehash
//  If this fails, the symbol table is untouched, and we return 2
// 0 - Success
// 1 - Key exists in table
int symtab_insert(symtab *t, ast_node *n)
{
    if((t->filled)/2 < max_cap)
    {
        int ret_val = rehash(symtab *t);
        if(ret_val == 2) { return 2; }
    }
}

// Check if symbol table contains key
// 0 - Not contained
// 1 - Contains key
int symtab_contains()
{

}

// Frees a given symbol table *t.
void symtab_free(symtab *t)
{

}
