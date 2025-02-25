#ifndef __LEXER_LEX_H_JR
#define __LEXER_LEX_H_JR

// yytext %array size
#define YYLMAX 4096

#define U_BIT 1
#define L_BIT 2
#define LL_BIT 4
#define D_BIT 8
#define DL_BIT 16
#define INVAL_BIT 32
#define FL_BIT 64

#define IS_UNSIGNED(tag) ((tag & U_BIT) == U_BIT)
#define IS_LONG(tag) ((tag & L_BIT) == L_BIT)
#define IS_LLONG(tag) ((tag & LL_BIT) == LL_BIT)
#define IS_DOUBLE(tag) ((tag & D_BIT) == D_BIT)
#define IS_DOUBLONG(tag) ((tag & DL_BIT) == DL_BIT)
#define IS_INVAL(tag) ((tag & INVAL_BIT) == INVAL_BIT)
#define IS_FLOATING(tag) ((tag & FL_BIT) == FL_BIT)

#define YYSTYPE struct yy_struct

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
};

#endif
