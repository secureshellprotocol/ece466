
    /* mid burger lexer -- james ryan */

%option yylineno
%option noyywrap
%array
%top{
#include <string.h>

#include "lex_utils.h"
#include "tokens.h"

// yytext %array size
#define YYLMAX 4096

// mega-mega-kludge

#define U_BIT 1
#define L_BIT 2
#define LL_BIT 4
#define D_BIT 8
#define DL_BIT 16

struct yy_struct{
union 
{
    // integer
//    int d;
//    unsigned int ud;
//    long int ld;
//    unsigned long int uld;
//    long long lld;
    unsigned long long int ulld;
    //  real
    float f;
    double df;
    long double ldf;
    //  charlit, ident or string
    char *s;
};
int tags;
/*  bit meaning (1 is true)
    0   unsigned
    1   long
    2   long long   (XOR bit 1)
    3   double
    4   long double
*/
};

#define YYSTYPE  struct yy_struct
//extern YYSTYPE yylval;
YYSTYPE yylval;


int line_num = 1;
char yyin_name[4096] = "<stdin>";   
char charliteral = '0';
char stringval[4096] = "\0";

char string_buf[4096];
char *string_buf_ptr;

}

    /* https://stackoverflow.com/questions/63785787/flex-regular-expression-for-strings-with-either-single-or-double-quotes */
string_lit_match    ([^"\\\n]|\\(.|\n))*
c_char_match  ([^'\\\n]|\\(.|\n))* 

hexadecimal_prefix  ^0[xX]{1}
hexadecimal_digit   [0-9A-Fa-f]+
hexadecimal_constant    [{hexadecimal_prefix}]|[{hexadecimal_constant}]

octal_prefix ^[0]+
octal_digit [0-7]+
octal_constant  [{octal_prefix}]|[{octal_digit}]+

nonzero_digit [1-9]
decimal_digit  [0-9]
decimal_constant    [{nonzero_digit}]|[{decimal_digit}]+

unsigned-suffix [uU]$
long-suffix [lL]$
long-long-suffix    l{2}$|L{2}$

ident       [_A-Za-z][_A-Za-z0-9]*

    // start conditions

%x  markermode
%x  markermode_s2

%x  c_char

%x  string_lit

%x  real

%x  hex
%x  oct
%x  dec

%%

"auto"          {return AUTO;}
"break"         {return BREAK;}
"case"          {return CASE;}
"char"          {return CHAR;}
"const"         {return CONST;}
"continue"      {return CONTINUE;}
"default"       {return DEFAULT;}
"do"            {return DO;}
"double"        {return DOUBLE;}
"else"          {return ELSE;}
"enum"          {return ENUM;}
"extern"        {return EXTERN;}
"float"         {return FLOAT;}
"for"           {return FOR;}
"goto"          {return GOTO;}
"if"            {return IF;}
"inline"        {return INLINE;}
"int"           {return INT;}
"long"          {return LONG;}
"register"      {return REGISTER;}
"restrict"      {return RESTRICT;}
"return"        {return RETURN;}
"short"         {return SHORT;}
"signed"        {return SIGNED;}
"sizeof"        {return SIZEOF;}
"static"        {return STATIC;}
"struct"        {return STRUCT;}
"switch"        {return SWITCH;}
"typedef"       {return TYPEDEF;}
"union"         {return UNION;}
"unsigned"      {return UNSIGNED;}
"void"          {return VOID;}
"volatile"      {return VOLATILE;}
"while"         {return WHILE;}
"_Bool"         {return BOOL;}
"_Complex"      {return COMPLEX;}
"_Imaginary"    {return IMAGINARY;}

"->"        {return INDSEL;}
"++"        {return PLUSPLUS;}
"--"        {return MINUSMINUS;}
"<<"        {return SHL;}
">>"        {return SHR;}
"<="        {return LTEQ;}
">="        {return GTEQ;}
"=="        {return EQEQ;}
"!="        {return NOTEQ;}
"&&"        {return LOGAND;}
"||"        {return LOGOR;}
"..."       {return ELLIPSIS;}
"*="        {return TIMESEQ;}
"/="        {return DIVEQ;}
"%="        {return MODEQ;}
"+="        {return PLUSEQ;}
"-="        {return MINUSEQ;}
"<<="       {return SHLEQ;}
">>="       {return SHREQ;}
"&="        {return ANDEQ;}
"|="        {return OREQ;}
"^="        {return XOREQ;}

\[      {return '[';}
\]      {return ']';}
"("     {return '(';}
")"     {return ')';}
"{"     {return '{';}
"}"     {return '}';}
"."     {return '.';}
"&"     {return '&';}
"*"     {return '*';}
"+"     {return '+';}
"-"     {return '-';}
"~"     {return '~';}
"!"     {return '!';}
"/"     {return '/';}
"%"     {return '%';}
"<"     {return '<';}
">"     {return '>';}
"|"     {return '|';}
"^"     {return '^';}
"?"     {return '?';}
":"     {return ':';}
";"     {return ';';}
","     {return ',';}
    
    /* marker mode */

"# "        {BEGIN(markermode);}
    /* stage 1 - mark line no */
	/* this is broken */
<markermode>[0-9]  {
    line_num = strtoull(yytext, NULL, 10);
    BEGIN(markermode_s2);
}
    /* stage 2 - mark file name */
<markermode_s2>" "  

<markermode_s2>{string_lit_match}  {
    strncpy(yyin_name, yytext, 4096);
}

<markermode_s2>\"  

<markermode_s2>[ \t\n ]+  {
    BEGIN(INITIAL);
}

    /* charlits */
    /* todo:*/
    /* work on detecting chars that exceed size */

"'"  {
    BEGIN(c_char);
}

    /* handle escape sequences */
<c_char>{c_char_match}   {
    yylval.s = strdup(yytext);
}

<c_char>"'" {
    BEGIN(INITIAL);
    return CHARLIT;
}

    /* strings */
    /* shamelessly ripped from Flex manual */

\"  {
    string_buf_ptr = string_buf;
    BEGIN(string_lit);
}

<string_lit>\"  {
    BEGIN(INITIAL);
    *string_buf_ptr = '\0';
    yylval.s = strdup(string_buf);

    return STRING;
}

<string_lit>[ \t\n ]+   {
    fprintf(stderr, \
"%s: Unterminated string constant at line %d: %s\n", yyin_name, line_num, yytext);
    exit(-1);
}

<string_lit>\\^0[0-7]{1,3}  {/* octal escape sequence */
    int result;

    (void) sscanf( yytext + 1, "%o", &result );

    if ( result > 0xff )
    {
        fprintf(stderr, \
    "%s: Line %d: %s is too large\n", yyin_name, line_num, yytext);
        exit(-1);   
    }

    *string_buf_ptr++ = result;
}

<string_lit>\\[0-9A-Za-z]{1,2}  {/* hex escape sequence */
    int result;

    (void) sscanf( yytext + 1, "%x", &result );

    if ( result > 0xff )
    {
        fprintf(stderr, \
    "%s: Line %d: %s is too large\n", yyin_name, line_num, yytext);
        exit(-1);   
    }

    *string_buf_ptr++ = result;
}

<string_lit>\\(.|\n)  *string_buf_ptr++ = yytext[1];

<string_lit>[^\\\n\"]+ {
    char *yptr = yytext;

    while ( *yptr )
        *string_buf_ptr++ = *yptr++;
}

    /* integers */

    /* HAK -- is it ok to start at the largest primitive (eg long long) */
    /*and work my way down to the datatype i need to be at based on the */ 
    /*L, U declarations?

    /*This feels hacky and also like i will lose precision.             */

    /*as you can tell, this was a rushed one.                           */ 

^[0]+[0-7]* { 
    yylval.ulld = strtoull(yytext, NULL, 8);
    BEGIN(oct);
}

[0-9]+[UL]* {
    BEGIN(dec);
    yylval.ulld = (long long int) strtoull(yytext, NULL, 10);
}


^0[xX]{1}[0-9A-Fa-f]+[UL]*   {
    yylval.ulld = strtoull(yytext, NULL, 16);
    ECHO;
	BEGIN(hex);
}

<dec,oct,hex>"LLU"$ {
    yylval.tags |= (LL_BIT | U_BIT);
    BEGIN(INITIAL);
    return NUMBER;
}

<dec,oct,hex>"LU"$  {
    yylval.tags |= (L_BIT | U_BIT);
    BEGIN(INITIAL);
    return NUMBER;
}

<dec,oct,hex>"LL"$  {
    yylval.tags |= (LL_BIT);
    BEGIN(INITIAL);
    return NUMBER;
}

<dec,oct,hex>"L"$  {
    yylval.tags |= (L_BIT);
    BEGIN(INITIAL);
    return NUMBER;
}

<dec,oct,hex>"U"$  {
    yylval.tags |= (U_BIT);
    BEGIN(INITIAL);
    return NUMBER;
}

<dec,oct,hex>.   {
    BEGIN(INITIAL);
    return NUMBER;
}


<hex>.  {
    fprintf(stderr, \
"%s: Unrecognized hex number at line %d: %s\n", yyin_name, line_num, yytext);
    BEGIN(INITIAL);
}
<oct>.  {
    fprintf(stderr, \
"%s: Unrecognized octal number at line %d: %s\n", yyin_name, line_num, yytext);
    BEGIN(INITIAL);
}
    /* identifier */
{ident}     {
    yylval.s = strdup(yytext);
    return IDENT;
}

[ \t\n ]+   ++line_num;
.           {fprintf(stderr, \
"%s: Unrecognized character at line %d: %s\n", yyin_name, line_num, yytext);
}

%%

int main(int argc, char* argv[])
{
    ++argv, --argc;  /* skip over program name */
    if ( argc > 0 )
    {
            yyin = fopen(argv[0], "r");
            strncpy(yyin_name, argv[0], 4096);
    }
    else
    {
            yyin = stdin;
    }
    int t;
    
    char text[256];
    while(t = yylex())
    {
        char *token_id = get_token_id(t);
        
        switch(t){
            case NUMBER:
                printf("%s\t%d\t%s\t%lld\n", 
                    yyin_name, 
                    line_num, 
                    token_id,
                    yylval.ulld);
                break;
            case CHARLIT:
                printf("%s\t%d\t%s\t%s\n", 
                    yyin_name, 
                    line_num, 
                    token_id,
                    yylval.s);
                break;
            case STRING:
                printf("%s\t%d\t%s\t%s\n", 
                    yyin_name, 
                    line_num, 
                    token_id,
                    yylval.s);
                break;
            case IDENT:
                printf("%s\t%d\t%s\t%s\n", 
                    yyin_name, 
                    line_num, 
                    token_id,
                    yylval.s);
                break;
            default:
                printf("%s\t%d\t%s\t%s\n", 
                    yyin_name, 
                    line_num, 
                    token_id,
                    yytext);
                break;  
        }
    }
}
