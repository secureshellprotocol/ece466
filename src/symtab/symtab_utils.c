#include <james_utils.h>
#include <ast/ast.h>
#include <symtab/symtab.h>

// wrapper around lookup to tell us if a specific node in our table is a 
//  VARIABLE, FUNCTION, LABEL
// It errors if it cant find our declaration
int decl_type(symbol_scope *s, char *n, int ns)
{
    return -1;
}
