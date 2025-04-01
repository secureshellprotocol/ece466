#ifndef __LEXER_LEX_H_JR
#define __LEXER_LEX_H_JR

#include <stdint.h>

// yytext %array size
#define YYLMAX  4096

// lexer hints
#define U_BIT   1     // unsigned
#define L_BIT   2     // long
#define LL_BIT  4     // long long
#define D_BIT   8     // double
#define F_BIT   16    // float

#define LEX_IS_UNSIGNED(tag) ((tag & U_BIT))
#define LEX_IS_LONG(tag) ((tag & L_BIT))
#define LEX_IS_LLONG(tag) ((tag & LL_BIT))
#define LEX_IS_DOUBLE(tag) ((tag & D_BIT))
#define LEX_IS_DOUBLONG(tag) ((tag & (D_BIT | L_BIT)))
#define LEX_IS_FLOATING(tag) ((tag & F_BIT))

// type qualifiers
#define TQ_CONST    32
#define TQ_RESTRICT 64
#define TQ_VOLATILE 128

#define IS_CONST(tag) ((tag & TQ_CONST))
#define IS_RESTRICT(tag) ((tag & TQ_RESTRICT))
#define IS_VOLATILE(tag) ((tag & TQ_VOLATILE))

// storage classes
#define SC_TYPEDEF  1
#define SC_EXTERN   2
#define SC_STATIC   4
#define SC_AUTO     8
#define SC_REGISTER 16

#define IS_TYPEDEF(tag) ((tag & SC_TYPEDEF))
#define IS_EXTERN(tag) ((tag & SC_EXTERN))
#define IS_STATIC(tag) ((tag & SC_STATIC))
#define IS_AUTO(tag) ((tag & SC_AUTO))
#define IS_REGISTER(tag) ((tag & SC_REGISTER))

// type specifiers
#define TS_VOID     1
#define TS_CHAR     2
#define TS_SHORT    4
#define TS_INT      8
#define TS_LONG     16
#define TS_FLOAT    32
#define TS_DOUBLE   64
#define TS_SIGNED   128
#define TS_UNSIGNED 256

#define IS_VOID(tag) ((tag & TS_VOID))
#define IS_CHAR(tag) ((tag & TS_CHAR))
#define IS_SHORT(tag) ((tag & TS_SHORT))
#define IS_INT(tag) ((tag & TS_INT))
#define IS_LONG(tag) ((tag & TS_LONG))
#define IS_FLOAT(tag) ((tag & TS_FLOAT))
#define IS_DOUBLE(tag) ((tag & TS_DOUBLE))
#define IS_SIGNED(tag) ((tag & TS_SIGNED))
#define IS_UNSIGNED(tag) ((tag & TS_UNSIGNED))

// invalid
#define INVAL_BIT   (1 << 31)

#define IS_INVAL(tag) ((tag & INVAL_BIT))

#define TAG_SET(tag, bitmask_constant) \
    ((tag |= bitmask_constant))
#define TAG_UNSET(tag, bitmask_constant) \
    ((tag &= ~(bitmask_constant)))

typedef struct ast_node_t ast_node;

/* #define YYSTYPE struct yy_struct */

struct yy_struct{
    union 
    {
        // integer
        unsigned long long int ulld;
        //  real
        long double ldf;
        //  charlit, ident or string
        char *s;
    };

    int s_len;   // Only used for strings
    uint32_t tags;  // lexer hints

    uint32_t stgclass;
    uint32_t typequals;
    uint32_t typespecs;

    ast_node *n;
};

#endif
