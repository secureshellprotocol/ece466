#ifndef __SYMTAB_ATTR_LIST_H_JR
#define __SYMTAB_ATTR_LIST_H_JR

#include <symtab/symtab.h>

typedef struct varattr_t {
    unsigned type_e;            // enum code of type
    unsigned storage_class_e;   // enum code of storage class
    int offset;
    int tags;   // tags -- only used for numbers
    union {
        long long int i_val;
        long double lf_val;
        char *s_val;
    };
} varattr;

typedef struct argtype_list_t {
    unsigned argtype_e;
    struct argtype_list_t *next;
} argtype_list;

typedef struct fcnattr_t {
    // bools
    int complete;       // incomplete (0), complete (1)
    int inline_bool;    // not inline (0), inline (1)

    unsigned type_e;
    argtype_list *argtypes;     // linked-list of argument types
} fcnattr;

typedef struct memberattr_t {
    unsigned type_e;
    int offset;
    
    unsigned bf_width;
    int bf_offset;
} memberattr;

typedef struct sueattr_t {
    int complete;       // incomplete (0), complete (1)
    
    symtab_elem *members;
} sueattr;

typedef struct enumconstattr_t {
    char *tag;
    unsigned value;
} enumconstattr;

typedef struct statementattr_t {
    char *tag;
} statementattr;

typedef struct attr_list_t{
    union {
        varattr v;
        fcnattr f;
        sueattr sue;
        enumconstattr e;
        statementattr st;
        memberattr m;
    };
} attr_list;

#endif
