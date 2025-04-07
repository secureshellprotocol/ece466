#include <string.h>

#include <symtab/symtab.h>

void symtabprint(symbol_scope *scope, char *name, int ns,c );


char *scopedecode(int ns)
{
    static char name[32] = {'\0'};

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
