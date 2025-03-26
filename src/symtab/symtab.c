#include <symtab/attr_list.h>
#include <symtab/symtab.h>

symbol_scope *symtab_create(symbol_scope *p)
{
    symbol_scope *s = calloc(1, sizeof(symbol_scope));
    s->identifiers = NULL;
    s->label_names = NULL;
    s->sue_tags = NULL;
    s->previous = p;
    
    return s;
}

void symtab_destroy(symbol_scope *s)
{
    free(s);
    return;
}

symtab_elem *symtab_lookup()
