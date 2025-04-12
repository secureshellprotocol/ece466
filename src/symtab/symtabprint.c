#include <string.h>

#include <james_utils.h>
#include <symtab/symtab.h>

void symtabprint(char *ident, char *file_name, unsigned int line_num, 
        ast_node *attrs, symbol_scope *scope)
{
    STDERR_F("%s is defined at %s:%u [in %s scope starting at %s:%u] as:\n",
            ident, file_name, line_num, scopedecode(scope->name), 
            scope->origin_file, scope->origin_lineno
            );
}

char *scopedecode(uint32_t scope)
{
    static char name[32];
    name[0] = '\0';

    switch(scope)
    {
        case SCOPE_GLOBAL:
            strcpy("global", name);
            return name;
        case SCOPE_FUNCTION:
            strcpy("function", name);
            return name;
        case SCOPE_SUE:
            strcpy("struct/union member", name);
            return name;
        default:
            strcpy("UNDEFINED", name);
            return name;
    }
}

char *nsdecode(int ns)
{
    static char name[32];
    name[0] = '\0';

    switch(ns)
    {
        case NS_LABELS:
            strcpy("label", name);
            return name;
        case NS_SUE:
            strcpy("struct/union", name);
            return name;
        case NS_IDENTS:
            strcpy("ident", name);
            return name;
        case NS_MEMBERS:
            strcpy("member", name);
            return name;
        default:
            strcpy("UNDEFINED", name);
            return name;
    }
}
