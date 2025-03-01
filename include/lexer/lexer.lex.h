#ifndef __LEXER_LEX_H_JR
#define __LEXER_LEX_H_JR

// yytext %array size
#define YYLMAX 4096

#define U_BIT 1
#define L_BIT 2
#define LL_BIT 4
#define D_BIT 8
#define F_BIT 16
#define INVAL_BIT 32

#define IS_UNSIGNED(tag) ((tag & U_BIT))
#define IS_LONG(tag) ((tag & L_BIT))
#define IS_LLONG(tag) ((tag & LL_BIT))
#define IS_DOUBLE(tag) ((tag & D_BIT))
#define IS_DOUBLONG(tag) ((tag & (D_BIT | L_BIT)))
#define IS_FLOATING(tag) ((tag & F_BIT))
#define IS_INVAL(tag) ((tag & INVAL_BIT))


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
    unsigned int tags;
    /*  bit meaning (1 is true)
        0   unsigned
        1   long
        2   long long   (XOR bit 1)
        3   double
        4   long double
        5   invalid
        6   floating
    */

    ast_node *n;
};

#endif
