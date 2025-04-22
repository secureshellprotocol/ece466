#ifndef __OP_H_JR
#define __OP_H_JR
// contains types of ast nodes and/or symtab elements
enum {
    UNAOP=400,
    BINOP,
    TERNOP,
    TYPECAST,
    FUNCTION,
    LIST,
    POINTER,
    ARRAY,
    DECLARATION,
    SUE_DECL,
    FNDEF,
    LABEL
};
#endif
