#include <string.h>

#include <james_utils.h>
#include <parser/op.h>
#include <symtab/symtab.h>

#define STPRINT(fmt, ...) \
    fprintf(stderr, fmt "\n", __VA_ARGS__);

void symtabprint(symbol_scope *scope, enum namespaces ns, char *l)
{
    symtab_elem *e = symtab_lookup(scope, l, ns);

    if(scope == NULL)
    {
        STDERR_F("Given scope invalid when looking up %s, ns %s!", 
                l, nsdecode(ns));
    }

    if(e == NULL)
    {
        STDERR_F("%s is not found in %s, ns %s",
                l, scopedecode(scope->scope), nsdecode(ns));
    }

    STPRINT("%s is defined at %s:%u [in %s scope starting at %s:%u] as a",
            l, e->file_origin, e->line_num_origin, 
            scopedecode(scope->scope), scope->origin_file, scope->origin_lineno);
            // TODO: maybe refactor symtab_scope to match symtab_elem? please?

    ast_node *dvar = e->d;
    STPRINT("%s with stgclass %s of type: ",
            nodetypedecode(dvar), 
            stgclassdecode(dvar->d.stgclass));
}

char *declspecsprinter(ast_node *decl_specs)
{

}

char *stgclassdecode(ast_node *n)
{
    static char name[32];
    name[0] = '\0';

    if(n == NULL)
    {
        strcpy(name, "NULL?");
        return name;
    }
       
    switch(n->op_type)
    {
        case TYPEDEF:
            strcpy(name, "typedef");
            return name; 
        case EXTERN:
            strcpy(name, "extern");
            return name;
        case STATIC:
            strcpy(name, "static");
            return name;
        case AUTO:
            strcpy(name, "auto");
            return name;
        case REGISTER:
            strcpy(name, "register");
            return name;
        default:
            strcpy(name, "<invalid>");
            astprint(n);
            return name;
    }

}

char *scopedecode(enum scopes s)
{
    static char name[32];
    name[0] = '\0';

    switch(s)
    {
        case SCOPE_GLOBAL:
            strcpy(name, "global");
            return name;
        case SCOPE_FUNCTION:
            strcpy(name, "function");
            return name;
        case SCOPE_SUE:
            strcpy(name, "struct/union member");
            return name;
        default:
            strcpy(name, "UNDEFINED");
            return name;
    }
}

char *nsdecode(enum namespaces ns)
{
    static char name[32];
    name[0] = '\0';

    switch(ns)
    {
        case NS_LABELS:
            strcpy(name, "label");
            return name;
        case NS_SUE:
            strcpy(name, "struct/union");
            return name;
        case NS_IDENTS:
            strcpy(name, "ident");
            return name;
        case NS_MEMBERS:
            strcpy(name, "member");
            return name;
        default:
            strcpy(name, "UNDEFINED");
            return name;
    }
}

char *nodetypedecode(ast_node *n)
{
    static char name[32];
    name[0] = '\0';

    switch(n->op_type)
    {
        case DECLARATION: // variable
            strcpy(name, "variable");
            return name;
        case FNDEF:
            strcpy(name, "function");
            return name;
        default:
            strcpy(name, "UNDEFINED");
            astprint(n);
            return name;
    }
}
