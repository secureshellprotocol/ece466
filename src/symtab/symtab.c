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


//int symtab_enter(symbol_scope *scope, char *name, enum namespaces ns, 
//        ast_node *v, char *file_origin, unsigned int line_no_origin)
//{ 
//    // generate elem
//    symtab_elem *new = calloc(1, sizeof(symtab_elem));
//
//    new->n = v;
//    new->key = strdup(name);
//    new->file_origin = strdup(file_origin);
//    new->line_no_origin = line_no_origin;
//
//    return 0;
//}
//
//int _symtab_inject_elem(symbol_scope *scope, enum namespaces ns, symtab_elem *e)
//{ 
//                
//    if(e->n->var.stgclass == NULL)
//    {
//        switch(e->n->var.i->op_type)
//        {
//            case IDENT: // scalar
//                switch(scope->scope)
//                {
//                    case SCOPE_GLOBAL:
//                        e->n->var.stgclass = ast_create_type(EXTERN);
//                        break;
//                    default:
//                        e->n->var.stgclass = ast_create_type(AUTO);
//                        break;
//                }
//                break;
//            case FUNCTION:
//                // todo add a check to make sure a function in block scope
//                // doesnt have a non-extern stgclass
//                e->n->var.stgclass = ast_create_type(EXTERN);
//                break;
//            default:
//                STDERR_F("stgclass check unimplemented for node type %d", 
//                        e->n->var.i->op_type);
//                break;
//        }
//    }
//
//    switch(ns)
//    {
//        case NS_LABELS:
//            e->next = scope->labels;
//            scope->labels = e;
//            break;
//        case NS_SUE:
//            e->next = scope->sue_tags;
//            scope->sue_tags = e;
//            break;
//        case NS_IDENTS:
//            if(scope->previous == NULL)
//            {
//                if(e->n->var.stgclass == NULL)
//                {
//                    e->n->var.stgclass = ast_create_type(EXTERN);
//                }
//            }
//
//            e->next = scope->idents;
//            scope->idents = e;
//            break;
//        case NS_MEMBERS:
//            // TODO: inherit from parent
//            STDERR("member symtables are unimplemented..");
//            return -1;
//        default:
//           STDERR_F("Failed to insert into namespace %d!", ns);
//           return -1;
//    }
//    return 0;
//}

void _symtab_install_var(symbol_scope *scope, 
        ast_node *stgclass, ast_node *decl_specs, ast_node *declarator,
        char *yyin_name, unsigned int line_num)
{
    symtab_elem *new = calloc(1, sizeof(symtab_elem));

    new->decl_specs = decl_specs;
    new->declarator = declarator;

    new->file_origin = strdup(yyin_name);
    new->line_num_origin = line_num;

    // key off based on declarator to find our ident 'Key' - to be used to index
    // our declaraed variable in the table
    ast_node *ident_key = declarator;
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

    new->key = strdup(ident_key->ident.value);
    if(ident_key->list.prev != NULL)                // incase of composite type
        ident_key->list.prev->list.next = NULL;     // unhook from declarator

    if(stgclass == NULL)
    {
        switch(scope->scope)
        {
            case SCOPE_GLOBAL:
                stgclass = ast_create_type(EXTERN);
                break;
            default:
                stgclass = ast_create_type(AUTO);
                break;
        }
    }

    if(verify_decl_specs(decl_specs) == 1)
    {
        STDERR_F("INVALID DECL SPECS FOR %s!", new->key);
        return;
    }

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

    symtabprint(scope, NS_IDENTS, new->key);
    return;
}


void symtab_install(symbol_scope *scope, ast_node *decl_specs, ast_node *decl_list,
        char *yyin_name, unsigned int line_num)
{
    if(scope == NULL || decl_specs == NULL || decl_list == NULL)
    {
        STDERR("Invalid arguments supplied!");
        return;
    }

    // retrieve stgclass
    ast_node *stgclass = NULL;
    
    while(1)
    {
        switch(decl_specs->list.value->op_type)
        {
            case TYPEDEF: case EXTERN: case STATIC: case AUTO: case REGISTER:
                if(stgclass != NULL)
                {
                    STDERR("Multiple storage classes specified! Found");
                    astprint(stgclass);
                    astprint(decl_specs->list.value);
                    STDERR(" when installing ");
                    astprint(decl_list->list.value);
                    return;
                }
                stgclass = decl_specs;
                stgclass->list.next = NULL;
                decl_specs = decl_specs->list.next;
                decl_specs->list.prev = NULL;
                break;
            default:
                goto stgclass_done;
        }
    }
stgclass_done: // its possible for stgclass to be NULL here -- we catch that
               // later

    /*      topology of man and society
     *
     *  [stgclass]
     *
     *  [decl_specs] -- [decl_list] ---- [decl_list]
     *                      |               |
     *                      [ident x]       [ptr]
     *                                      |
     *                                      [ident x]
     */

    while(decl_list != NULL)
    {
        _symtab_install_var(
                scope,
                stgclass,
                decl_specs,
                decl_list->list.value,
                yyin_name,
                line_num
                );
        decl_list = decl_list->list.next;
    }

}
