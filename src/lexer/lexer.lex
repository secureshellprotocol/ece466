%option yylineno
%option noyywrap
%array

%top{
#include <ctype.h>
#include <string.h>

#include <ast/ast.h>
#include <lexer/lexer.lex.h>
#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>

#ifdef __LEXER_STANDALONE_H_JR
YYSTYPE yylval;
#else
extern YYSTYPE yylval;
#endif

int line_num = 1;
char yyin_name[4096] = "<stdin>";   
char charliteral = '0';
char stringval[4096] = "\0";

char string_buf[4096];
char *string_buf_ptr;
char *string_start;
}

    /* https://stackoverflow.com/questions/63785787/flex-regular-expression-for-strings-with-either-single-or-double-quotes */
string_lit_match    ([^"\\\n]|\\(.|\n))*

hex_real    0[xX]{1}(([0-9A-Fa-f]+\.+[0-9A-Fa-f]*)|([0-9A-Fa-f]*\.+[0-9A-Fa-f]+))+([pP]{0,1}[+-]{0,1}[0-9]+)*
dec_real    ((([0-9]*\.+[0-9]+)|([0-9]+\.+[0-9]*))+([eE]{0,1}[+-]{0,1}[0-9]+)*)

ident       [_A-Za-z][_A-Za-z0-9]*

    // start conditions

%x  markermode
%x  markermode_s2

%x  c_char

%x  string_lit

%x  integer
%x  real_exponent
%x  real_suffix

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
"="     {return '=';}

    /* marker mode */

"# "        {BEGIN(markermode);}
    /* stage 1 - mark line no */
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

<markermode_s2>[ \n ]+  {
    BEGIN(INITIAL);
}

"'"  {
    string_buf_ptr = string_buf;
    
    string_start = string_buf_ptr;
    BEGIN(c_char);
}

<c_char>"'" {
    int string_length = (int) (string_buf_ptr - string_start);
    if(string_length > 1){
        fprintf(stderr,
"\nlexer:When the going gets tough, everyone leaves.\n\
lexer: %s: line %d: %s cannot fit in a char! concatenated to %c\n", yyin_name, line_num, string_buf, string_buf[0]);
        
        yylval.s_len = 1;
    } else yylval.s_len = string_length;

    string_buf[1] = '\0';
    yylval.s = strdup(string_buf);
    
    BEGIN(INITIAL);
    return CHARLIT;
}

\"  {
    string_buf_ptr = string_buf;
    
    string_start = string_buf_ptr;
    BEGIN(string_lit);
}

<string_lit>\"  {
    *string_buf_ptr = '\0';
    yylval.s_len = (int) (string_buf_ptr - string_start);
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
    "lexer:When the going gets tough, everyone leaves. \n\
    lexer: %s: Line %d: %s is too large\n", yyin_name, line_num, yytext);
        exit(-1);   
    }
    *string_buf_ptr++ = result;
}

<c_char,string_lit>\\[xX]{1}[0-9A-Za-z]{1,2}  {/* hex escape sequence */
    unsigned long long int result;
    result = strtoull(yytext + 2, NULL, 16);

    if ( result > 0xff )
    {
        fprintf(stderr, \
    "lexer:When the going gets tough, everyone leaves.\n\
    lexer: %s: Line %d: %s is too large\n", yyin_name, line_num, yytext);
        exit(-1);   
    }
    *string_buf_ptr++ = result;
}

<c_char,string_lit>\\(.|\n)  { 
    switch(yytext[1])
    {
        case '0':
            *string_buf_ptr++ = '\0';
            break;
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
"lexer: When the going gets tough, everyone leaves.\n\
lexer: %s: Unterminated string constant at line %d: %s\n", yyin_name, line_num, yytext);
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


    /* reals and integers */

{dec_real}|{hex_real} {
    yylval.ldf = strtold(yytext, NULL);
    yylval.tags = (F_BIT | D_BIT);    // default double precision on constants.
    BEGIN(real_suffix);
}

[1-9]+[0-9]* {
    yylval.ulld = strtoull(yytext, NULL, 10);
    yylval.tags = 0;
    BEGIN(integer);
}

[0]+[0-7]* { 
    yylval.ulld = strtoull(yytext, NULL, 8);
    yylval.tags = 0;
    BEGIN(integer);
}

0[xX]{1}[0-9A-Fa-f]+    {  
    yylval.ulld = strtoull(yytext, NULL, 16);
    yylval.tags = tagparse(yytext, 0);
    BEGIN(integer);
}

<real_suffix>[f]{1}|[F]{1} {
    yylval.tags = tagparse(yytext, yylval.tags);
}

<integer>[u]{1}|[U]{1}  {
    yylval.tags = tagparse(yytext, yylval.tags);
}

<real_suffix,integer>[l]{1}|[L]{1} {
    yylval.tags = tagparse(yytext, yylval.tags);
}


<real_suffix>[^lLfF]{1} {
/*   Note the 'sus' solution here -- intention! i matched one character which */
/*     wasnt anything that I was looking for. So. push it back, and pay it    */
/*     forward to the main lexer rules.                                       */
/*     this appears to do what i want. Note I'm using the %array flex option, */ 
/*     see section 8 of the Flex manual on unput()  */
    unput(yytext[0]);
    BEGIN(INITIAL);
    return NUMBER;
}
<integer>[^uUlL]{1} {
    unput(yytext[0]);
    BEGIN(INITIAL);
    return NUMBER;
}

    /* identifier */
{ident}     {
    yylval.s = strdup(yytext);
    return IDENT;
}

[\n]+   {++line_num;}
[ \t]+  { /*whitespace consumption */ } 

.           {fprintf(stderr, \
"lexer: When the going gets tough, everyone leaves.\n\
lexer: %s: Unrecognized character at line %d: %s\n", yyin_name, line_num, yytext);
}

%%

#ifdef __LEXER_STANDALONE_H_JR
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
    
    while( (t = yylex()) )
    {
        char *token_id = get_token_id(t);
        
        printf("%s\t%d\t%s\t",
            yyin_name,
            line_num,
            token_id);

        switch(t){
            case NUMBER:
                if(IS_INVAL(yylval.tags))
                {
                    printf( "INVALID");
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
                break;
            case CHARLIT:
                chardecode(yylval.s[0]);
                break;
            case STRING:
                for(int i = 0; i < yylval.s_len; i++)
                {
                    chardecode(yylval.s[i]);
                }
                break;
            case IDENT:
                printf("%s", 
                    yylval.s);
                break;
            default:
                printf("%s", 
                    yytext);
                break;  

        }
        printf("\ttoken code: %d\n", t); 
    }
    return 0;
}
#else
// main is somewhere else.
#endif
