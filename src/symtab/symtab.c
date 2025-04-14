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

symbol_scope *symtab_create(symbol_scope *p, enum scopes scope_type,
        char *origin_file, uint32_t origin_lineno)
{
    symbol_scope *s = calloc(1, sizeof(symbol_scope));
    s->scope = scope_type;
    //s->name = strdup(name);

    s->origin_file = strdup(origin_file);
    s->origin_lineno = origin_lineno;

    s->idents = NULL;
    s->labels = NULL;
    s->sue_tags = NULL;
    
    s->previous = p;
    
    return s;
}

void symtab_destroy(symbol_scope *s)
{
    symtab_elem *ns_list[] = {
        s->idents,
        s->labels,
        s->sue_tags
    };

    for(int i = 0; i < NUM_ELEMS(ns_list); i++)
    {
        symtab_elem *current = ns_list[i];
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
    symtab_elem *e;
    switch(ns)
    {
        case NS_LABELS:
            e = scope->labels;
            break;
        case NS_SUE:
            e = scope->sue_tags;
            break;
        case NS_IDENTS:
            e = scope->idents;
            break;
        case NS_MEMBERS:
            STDERR("member symtables are unimplemented..");
            return NULL;
        default:
           STDERR_F("Failed to switch into namespace %d when looking up %s!", ns, name);
           return NULL;
    }
    
    while(e != NULL)
    {
        switch(e->n->op_type)
        {
            case VARIABLE:  // key off based on name
                            // switch based on type (ident, func, etc)
                            // TODO
                            // IN ENTER FUNCTIONS, ADD A POINTER TO THIS VALUE
                            // WHICH WE KEY OFF OF FOR ANY GIVEN BULLSHIT SO I
                            // NEVER HAVE TO DO THIS AGAIN
                if( (strcmp(e->n->var.i->ident.value, name)) == 0 )
                {
                    return e;
                }
                break;
            default:
                STDERR_F("not implemented for %d",
                        e->n->op_type);
        }
    }
    return NULL;    // no match found.
}


int symtab_enter(symbol_scope *scope, char *name, enum namespaces ns, 
        ast_node *v, char *file_origin, unsigned int line_no_origin)
{ 
    symtab_elem *ns_list[] = {
        scope->idents,
        scope->labels,
        scope->sue_tags
    };

    int selected_ns = -1;

    switch(ns)
    {
        case NS_LABELS:
            selected_ns = 0;
            break;
        case NS_SUE:
            selected_ns = 2;
            break;
        case NS_IDENTS:
            selected_ns = 1;
            break;
        case NS_MEMBERS:
            break;
        default:
            STDERR_F("Undefined namespace %d", ns);
    }

    // generate elem
    symtab_elem *new = calloc(1, sizeof(symtab_elem));

    new->n = v;
    new->key = strdup(name);
    new->file_origin = strdup(file_origin);
    new->line_no_origin = line_no_origin;

    return 0;
}

int _symtab_inject_elem(symbol_scope *scope, enum namespaces ns, symtab_elem *e)
{ 
                
    if(e->n->var.stgclass == NULL)
    {
        switch(e->n->var.i->op_type)
        {
            case IDENT: // scalar
                switch(scope->scope)
                {
                    case SCOPE_GLOBAL:
                        e->n->var.stgclass = ast_create_type(EXTERN);
                        break;
                    default:
                        e->n->var.stgclass = ast_create_type(AUTO);
                        break;
                }
                break;
            case FUNCTION:
                // todo add a check to make sure a function in block scope
                // doesnt have a non-extern stgclass
                e->n->var.stgclass = ast_create_type(EXTERN);
                break;
            default:
                STDERR_F("stgclass check unimplemented for node type %d", 
                        e->n->var.i->op_type);
                break;
        }
    }

    switch(ns)
    {
        case NS_LABELS:
            e->next = scope->labels;
            scope->labels = e;
            break;
        case NS_SUE:
            e->next = scope->sue_tags;
            scope->sue_tags = e;
            break;
        case NS_IDENTS:
            if(scope->previous == NULL)
            {
                if(e->n->var.stgclass == NULL)
                {
                    e->n->var.stgclass = ast_create_type(EXTERN);
                }
            }

            e->next = scope->idents;
            scope->idents = e;
            break;
        case NS_MEMBERS:
            // TODO: inherit from parent
            STDERR("member symtables are unimplemented..");
            return -1;
        default:
           STDERR_F("Failed to insert into namespace %d!", ns);
           return -1;
    }
    return 0;
}

// dude theres like a million layers of indirection here.
// figure the fuck out
void symtab_install(symbol_scope *scope, ast_node *n)
{
    n = ast_list_reverse(n);

    char *key;                          // symtab key
    enum namespaces installed_ns = -1;  // installed namespace
    ast_node *v = NULL;
    
    switch(n->list.value->op_type)  
    {
        case IDENT: // variable
            {
                v = ast_create_var(n);
                
                key = v->var.i->ident.value;
                installed_ns = NS_IDENTS;
            }
            break;
        default:    
error:
            STDERR_F("Failed to install op %d", n->list.value->op_type);
            astprint(n);
            return;
    }
    
    if(v == NULL)
    {
        STDERR_F("Failed to install %s into symtab!",
                n->list.value->ident.value);
        goto error;
    }
    
    if(symtab_lookup(scope, name, ns))
    {
        STDERR_F("%s already in scope %s!", name, nsdecode(ns));
        goto error;
    }


    symtab_enter(
        scope,
        key,
        installed_ns,
        v,
        yyin_name,
        line_num
    );

    symtabprint(scope, installed_ns, key);
    return;
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
