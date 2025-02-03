%option noyywrap
%top{
#include "tokens.h"

typedef union {
    char *string_literal;
    int integer;
    long long_int;
    float floating_pt;
    double double_pt;
} YYSTYPE;
extern YYSTYPE yylval;

// TOKEN METADATA
char *filename;
int line_no;

}


%%



%%

int main()
{
    int tok;
    
    while(tok=yylex())
    {

    }
}

