#include <string.h>

#include <james_utils.h>
#include <symtab/symtab.h>

#define PRINT(fmt, ...) \
    fprintf(stderr, fmt "\n", __VA_ARGS__);

void symtabprint(symbol_scope *scope, enum namespaces ns, char *l)
{
    symtab_elem *e = symtab_lookup(scope, l, ns);

    if(e != NULL)
    {
        astprint(e->declarator);
        astprint(e->decl_specs);
        astprint(e->stgclass);
    }
    else
    {
        STDERR_F("%s not found in %s", l, nsdecode(ns));
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
