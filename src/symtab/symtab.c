#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <james_utils.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>
#include <symtab/symtab.h>

#define NUM_ELEMS(x) (sizeof(x)/sizeof(x[0]))

symbol_scope *symtab_create(symbol_scope *p)
{
    symbol_scope *s = calloc(1, sizeof(symbol_scope));
    s->idents = NULL;
    s->labels = NULL;
    s->sue_tags = NULL;
    
    s->previous = p;
    
    return s;
}

void symtab_destroy(symbol_scope *s)
{
    symtab_elem *scope_list[] = {
        s->idents,
        s->labels,
        s->sue_tags
    };

    for(int i = 0; i < NUM_ELEMS(scope_list); i++)
    {
        symtab_elem *current = scope_list[i];
        while(current != NULL)
        {
            symtab_elem *t = current;
            current = t->next;
            free(t);
        }
    }

    free(s);
    return;
}

symtab_elem *symtab_lookup(symbol_scope *scope, char *name, int ns)
{
    symtab_elem *s;
    switch(ns)
    {
        case NS_LABELS:
            s = scope->labels;
            break;
        case NS_SUE:
            s = scope->sue_tags;
            break;
        case NS_IDENTS:
            s = scope->idents;
            break;
        case NS_MEMBERS:
            STDERR("member symtables are unimplemented..");
            return NULL;
        default:
           STDERR_F("symtab: Failed to switch into namespace %d when looking up %s!", ns, name);
           return NULL;
    }
    
    while(s != NULL)
    {
        if( (strcmp(s->name, name)) == 0 )
        {
            return s;
        }
    }
    return NULL;    // no match found.
}

int symtab_enter(symbol_scope *scope, char *name, int ns, ast_node *decl_specs, 
        char *file_origin, unsigned int line_no_origin)
{
    // generate elem
    symtab_elem *new = calloc(1, sizeof(symtab_elem));

    new->file_origin = strdup(file_origin);
    new->line_no_origin = line_no_origin;

    new->name = strdup(name);

    switch(ns)
    {
        case NS_LABELS:
            new->next = scope->labels;
            scope->labels = new;
            break;
        case NS_SUE:
            new->next = scope->sue_tags;
            scope->sue_tags = new;
            break;
        case NS_IDENTS:
            new->next = scope->idents;
            scope->idents = new;
            break;
        case NS_MEMBERS:
            STDERR("member symtables are unimplemented..");
            return -1;
        default:
           STDERR_F("symtab: Failed to insert %s into namespace %d!", name, ns);
           return -1;
    }
    
    return 0;
}

void symtab_install(symbol_scope *scope, ast_node *n)
{
    switch(n->op_type)  
    {
        case DECLARATION:
            symtab_install_decl(scope, n);
            break;
        default:
            STDERR_F("Failed to install op %d", n->op_type)
            break;
    }
}

void symtab_install_decl(symbol_scope *scope, ast_node *d)
{
    while(d->decl.decl_list != NULL && (d->decl.decl_list->list.next != NULL || d->decl.decl_list->list.value->op_type != IDENT ))
    {
        ast_list_insert(d->decl.decl_specs, d->decl.decl_list->list.value);
         

        d->decl.decl_list = d->decl.decl_list->list.next;
    }

    astprint(d);
    return;
}
