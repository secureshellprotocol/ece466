#ifndef __LEX_UTILS_H_JR
#define __LEX_UTILS_H_JR
void chardecode(char input);
unsigned int tagparse(const char* yytext, unsigned int yytags);
char *get_token_id(int token_code);
#endif
