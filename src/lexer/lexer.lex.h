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

#endif
