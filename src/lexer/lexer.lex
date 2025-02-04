%option noyywrap
%top{
#include <string.h>

#include "lex_utils.h"
#include "tokens.h"

// kludge
#define YYSTYPE char *
extern YYSTYPE yylval;
}

digits	[0-9]+
nondigits	[A-Za-z_]+

string	\"[^\n"]+\""]

ident		[_A-Za-z][_A-Za-z0-9]*

	int line_num = 1;
	char yyin_name[4096] = "<stdin>";	
	char charliteral = '0';
	char stringval[4096] = "\0";

%x	charlit
%x	comment

%%

"auto"			{return	AUTO;}
"break"			{return	BREAK;}
"case"			{return	CASE;}
"char"			{return	CHAR;}
"const"			{return	CONST;}
"continue"		{return	CONTINUE;}
"default"		{return	DEFAULT;}
"do"			{return	DO;}
"double"		{return	DOUBLE;}
"else"			{return	ELSE;}
"enum"			{return	ENUM;}
"extern"		{return	EXTERN;}
"float"			{return	FLOAT;}
"for"			{return	FOR;}
"goto"			{return	GOTO;}
"if"			{return	IF;}
"inline"		{return	INLINE;}
"int"			{return	INT;}
"long"			{return	LONG;}
"register"		{return	REGISTER;}
"restrict"		{return	RESTRICT;}
"return"		{return	RETURN;}
"short"			{return SHORT;}
"signed"		{return	SIGNED;}
"sizeof"		{return	SIZEOF;}
"static"		{return	STATIC;}
"struct"		{return	STRUCT;}
"switch"		{return	SWITCH;}
"typedef"		{return	TYPEDEF;}
"union"			{return	UNION;}
"unsigned"		{return	UNSIGNED;}
"void"			{return	VOID;}
"volatile"		{return	VOLATILE;}
"while"			{return	WHILE;}
"_Bool"			{return	BOOL;}
"_Complex"		{return	COMPLEX;}
"_Imaginary"	{return	IMAGINARY;}

"->"		{return	INDSEL;}
"++"		{return	PLUSPLUS;}
"--"		{return	MINUSMINUS;}
"<<"		{return	SHL;}
">>"		{return	SHR;}
"<="		{return	LTEQ;}
">="		{return	GTEQ;}
"=="		{return	EQEQ;}
"!="		{return	NOTEQ;}
"&&"		{return	LOGAND;}
"||"		{return	LOGOR;}
"..."		{return	ELLIPSIS;}
"*="		{return	TIMESEQ;}
"/="		{return	DIVEQ;}
"%="		{return	MODEQ;}
"+="		{return	PLUSEQ;}
"-="		{return	MINUSEQ;}
"<<="		{return	SHLEQ;}
">>="		{return	SHREQ;}
"&="		{return	ANDEQ;}
"|="		{return	OREQ;}
"^="		{return	XOREQ;}

"["		{return '[';}
"]"		{return ']';}
"("		{return '(';}
")"		{return ')';}
"{"		{return '{';}
"}"		{return '}';}
"."		{return '.';}
"&"		{return '&';}
"*"		{return '*';}
"+"		{return '+';}
"-"		{return '-';}
"~"		{return '~';}
"!"		{return '!';}
"/"		{return '/';}
"%"		{return '%';}
"<"		{return '<';}
">"		{return '>';}
"|"		{return '|';}
"^"		{return '^';}
"?"		{return '?';}
":"		{return ':';}
";"		{return ';';}
","		{return ',';}
"#"		{return '#';}


"/*"	BEGIN(comment);

<comment>[^*\n]*		/* eat any non-stars*/
<comment>"*"+[^*/\n]*	/* eat any stars not followed by forward slash*/
<comment>\n				++line_num;
<comment>"*"+"/"		BEGIN(INITIAL);

{digits}		{return	NUMBER;}

{ident}			{return IDENT;}

[ \t\n ]+	++line_num;
.			{fprintf(stderr, \
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
		strncpy(text, yytext, 256);
		
		switch(t){
			case CHARLIT:
				printf("%s\t%d\t%s\t%s\n", 
					yyin_name, 
					line_num, 
					token_id,
					charliteral);
				break;
			default:
				printf("%s\t%d\t%s\t%s\n", 
					yyin_name, 
					line_num, 
					token_id,
					text);
				break;	
		}
	}
}
