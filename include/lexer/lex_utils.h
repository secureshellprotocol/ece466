#ifndef __LEX_UTILS_H_JR
#define __LEX_UTILS_H_JR

#include <stdint.h>

void chardecode(char input);
unsigned int tagparse(const char* yytext, unsigned int yytags);
char *get_token_id(int token_code);
uint32_t tagappend(uint32_t tags, uint32_t new_tag);
#endif
