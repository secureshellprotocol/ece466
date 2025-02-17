
    /* mid burger lexer -- james ryan */

%option yylineno
%option noyywrap
%array
%top{
#include <ctype.h>
#include <string.h>

#include "lexer.lex.h"
#include "lex_utils.h"
#include "tokens.h"

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
    /* figure out how to display "\0" like c wants???? */

"'"  {
    string_buf_ptr = string_buf;
    BEGIN(c_char);
}

<c_char>"'" {
    if(strlen(string_buf) > 1){
        fprintf(stderr,
"\n%s: line %d: %s cannot fit in a char! concatenated to %c\n", yyin_name, line_num, string_buf, string_buf[0]);
    }
    
    string_buf[1] = '\0';
    yylval.s = strdup(string_buf);
    
    BEGIN(INITIAL);
    return CHARLIT;
}

\"  {
    string_buf_ptr = string_buf;
    BEGIN(string_lit);
}

<string_lit>\"  {
    *string_buf_ptr = '\0';
    yylval.s = strdup(string_buf);

    BEGIN(INITIAL);
    return STRING;
}


<c_char,string_lit>\\0[0-7]{1,3}  {/* octal escape sequence */
    unsigned long long int result;
    result = strtoull(yytext + 1, NULL, 8);

    if ( result > 0xff )
    {
        fprintf(stderr, \
    "%s: Line %d: %s is too large\n", yyin_name, line_num, yytext);
        exit(-1);   
    }

    *string_buf_ptr++ = result;
}

<c_char,string_lit>\\[xX]{1}[0]*[0-9A-Za-z]{1,2}  {/* hex escape sequence */
    unsigned long long int result;
    result = strtoull(yytext + 2, NULL, 16);

    if ( result > 0xff )
    {
        fprintf(stderr, \
    "%s: Line %d: %s is too large\n", yyin_name, line_num, yytext);
        exit(-1);   
    }

    *string_buf_ptr++ = result;
}

<c_char,string_lit>\\(.|\n)  { 
    switch(yytext[1])
    {
        case '0':
            *string_buf_ptr++ = '\0';
        case 'a':
            *string_buf_ptr++ = '\a';
            break;
        case 'b': 
            *string_buf_ptr++ = '\b';
            break;
        case 'f':
            *string_buf_ptr++ = '\f';
            break;
        case 'n':
            *string_buf_ptr++ = '\n';
            break;
        case 'r':
            *string_buf_ptr++ = '\r';
            break;
        case 't':
            *string_buf_ptr++ = '\t';
            break;
        case 'v':
            *string_buf_ptr++ = '\v';
            break;
        default:    /* copy literal value */
            *string_buf_ptr++ = yytext[1];
    }
}

<c_char,string_lit>[\n]+   {
    fprintf(stderr, \
"%s: Unterminated string constant at line %d: %s\n", yyin_name, line_num, yytext);
    exit(-1);
}

<c_char>[^\\\n\']+ {
    char *yptr = yytext;

    while ( *yptr )
        *string_buf_ptr++ = *yptr++;
}

<string_lit>[^\\\n\"]+ {
    char *yptr = yytext;

    while ( *yptr )
        *string_buf_ptr++ = *yptr++;
}


    /* integers */

[0]+[0-7]* { 
    yylval.ulld = strtoull(yytext, NULL, 8);
    yylval.tags = tagparse(yytext);
    return NUMBER;
}

[1-9]+[0-9]*[Uu]{0,1}[Ll]{0,2} {
    yylval.ulld = strtoull(yytext, NULL, 10);
    yylval.tags = tagparse(yytext);
    return NUMBER;
}


0[xX]{1}[0-9A-Fa-f]+[Uu]{0,1}[Ll]{0,2}   {
    yylval.ulld = strtoull(yytext, NULL, 16);
    yylval.tags = tagparse(yytext);
    return NUMBER;
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
        
        printf("%s\t%d\t%s\t",
            yyin_name,
            line_num,
            token_id);

        switch(t){
            case NUMBER:
                printf("%s\t%d\t%s\t", 
                    yyin_name, 
                    line_num, 
                    token_id);
                if(IS_INVAL(yylval.tags))
                {
                    printf( "INVALID\n");
                    break;
                }
                if(IS_FLOATING(yylval.tags))
                {
                    printf(" REAL %Lg ", yylval.ldf);
                }
                else printf(" INTEGER %lld ", yylval.ulld);
                if(IS_UNSIGNED(yylval.tags))
                {
                    printf(" UNSIGNED ");
                }
                if(IS_LONG(yylval.tags))
                {
                    printf(" LONG ");
                }
                if(IS_LLONG(yylval.tags))
                {
                    printf(" LONG LONG ");
                }
                if(IS_DOUBLE(yylval.tags))
                {
                    printf(" DOUBLE ");
                }
                if(IS_DOUBLONG(yylval.tags))
                {
                    printf(" LONG DOUBLE ");
                }
                printf("\n");
                break;
            case CHARLIT:
                switch(yylval.s[0])
                {
                    case 0:
                        printf("%s", "\\0");
                        break;
                    case '\a':
                        printf("%s", "\\a");
                        break;
                    case '\b': 
                        printf("%s", "\\b");
                        break;
                    case '\f':
                        printf("%s", "\\f");
                        break;
                    case '\n':
                        printf("%s", "\\n");
                        break;
                    case '\r':
                        printf("%s", "\\r");
                        break;
                    case '\t':
                        printf("%s", "\\t");
                        break;
                    case '\v':
                        printf("%s", "\\v");
                        break;
                    default:
                        if(isprint(yylval.s[0]))
                        {
                            printf("%c", yylval.s[0]);
                        }
                        else printf("\\%03o", yylval.s[0]);
                }
                printf("\n");
                break;
            case STRING:
                printf("%s\n", 
                    yylval.s);
                break;
            case IDENT:
                printf("%s\n", 
                    yylval.s);
                break;
            default:
                printf("%s\n", 
                    yytext);
                break;  
        }
    }
}
