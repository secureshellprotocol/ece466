#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <ast/types.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>
#include <symtab/symtab.h>

#define NUM_ELEMS(x) (sizeof(x)/sizeof(x[0]))

// src/lexer/lexer.lex
extern int line_num;
extern char yyin_name[4096];

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

    if(scope->previous == NULL)
    {
        decl_specs = ast_list_insert(
                decl_specs, ast_create_type(EXTERN)
            );
    }

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
    n = ast_list_reverse(n);

    switch(n->list.value->op_type)  
    {
        case IDENT: //scalar
        {
            char *key = n->list.value->ident.value;
            ast_node *attrs = n->list.next;

            symtabprint(
                    key,
                    yyin_name,
                    line_num,
                    attrs,
                    scope
                    );
            
            astprint(attrs);
            symtab_enter(
                    scope,
                    n->list.value->ident.value,
                    NS_IDENTS,
                    n->list.next,
                    yyin_name,
                    line_num
                    );
        }
            break;
        default:
            STDERR_F("Failed to install op %d", n->list.value->op_type);
            break;
    }
}

void symtab_install_list(symbol_scope *scope, ast_node *l)
{
    ast_node *iter = l;

    while(iter != NULL)
    {
        symtab_install(scope, iter->list.value);
    }

    return;
}
