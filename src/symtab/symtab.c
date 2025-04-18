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

symbol_scope *symtab_destroy(symbol_scope *s)
{
    symbol_scope *previous = s->previous;
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
    return previous; 
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
        if(strcmp(name, e->key) == 0)
        {
            return e;
        }
        e = e->next;
    }

    return NULL;    // no match found.
}

// install a variable into our symbol table
// meant to feed in from the symtab_install call
void _symtab_install_var(symbol_scope *scope, ast_node *decl,
        char *yyin_name, unsigned int line_num)
{
    // clean up stgclass
    if(decl->d.stgclass == NULL)
    {
        switch(scope->scope)
        {
            case SCOPE_GLOBAL:
                decl->d.stgclass = ast_create_type(EXTERN);
                break;
            default:
                decl->d.stgclass = ast_create_type(AUTO);
                break;
        }
    }

    symtab_elem *new = calloc(1, sizeof(symtab_elem));

    new->d = decl;

    new->file_origin = strdup(yyin_name);
    new->line_num_origin = line_num;

    // key off based on declarator to find our ident 'Key' - to be used to index
    // our declaraed variable in the table
    ast_node *ident_key = decl->d.declarator;
    while(ident_key != NULL && ident_key->list.value->op_type != IDENT)
    {
        ident_key = ident_key->list.next;
    }
    if(ident_key == NULL)
    {
        STDERR_F("Identifier not specified! bailing from %s:%d while init'ing var",
                yyin_name, line_num);
        return;
    }

    new->key = strdup(ident_key->list.value->ident.value);
    
    if(ident_key->list.prev != NULL)                // incase of composite type
        ident_key->list.prev->list.next = NULL;     // unhook from declarator
    
    // inject into ident namespace
    new->next = scope->idents;
    scope->idents = new;
    
    // verify we find it
    symtab_elem *confirm;
    if( (confirm = symtab_lookup(scope, new->key, NS_IDENTS)) == NULL )
    {
        STDERR_F("Failed to install variable %s into ident symbol table!", new->key);
        return;
    }

    symtabprint(scope, NS_IDENTS, confirm->key);
    return;
}


void symtab_install(symbol_scope *scope, ast_node *decl_list,
        char *yyin_name, unsigned int line_num)
{
    if(decl_list == NULL)
    {
        STDERR("Given a null declaration!");
        return;
    }
    
    while(decl_list != NULL)
    {
        ast_node *li = decl_list->list.value;
        switch(li->op_type)
        {
            case DECLARATION:   // variable
                _symtab_install_var(
                        scope,
                        li,
                        yyin_name,
                        line_num
                        );
                break;
            case FNDEF:         // function
                STDERR("FNDEF not defined");
                break;
            default:
                STDERR_F("Cannot install ast node of type %d to symtab!",
                        li->op_type);
                astprint(li);
                break;
        }
        decl_list = decl_list->list.next;
    }

    return;
}
