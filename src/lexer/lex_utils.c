#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "lexer.lex.h"
#include "tokens.h"

// prints a char if we can print it -- debug purposes.
// otherwise, spit out a code
void chardecode(char input)
{
    switch(input)
    {
        case '\0':
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
            if(isprint(input))
            {
                printf("%c", input);
            }
            else printf("\\%03o", input);
    }
}

unsigned int tagparse(char* yytext)
{
    char *numberlit = yytext + strlen(yytext) - 1;
    unsigned int tags = 0;

    while(numberlit >= yytext)
    {
        switch(*numberlit)
        {
            case 'U':
            u_suffix:
                tags |= U_BIT;
                break;
            case 'u':
                goto u_suffix;
            case 'L':
            l_suffix:
                if((tags &= L_BIT) == L_BIT)
                {
                    tags |= LL_BIT;
                    tags &= ~(L_BIT);
                    break;
                }
                tags |= L_BIT;
                break;
            case 'l':
                goto l_suffix;
            default:
                return tags;
        }
        numberlit--;
    }

    return tags;
}

// Simple switch-case map to find corresponding string code for a token
char *get_token_id(int token_code) 
{
    static char id[16];

    if(token_code > IMAGINARY) {
        return NULL;
    }

    switch(token_code) {
        case TOKEOF:
        case IDENT:
            strncpy(id, "IDENT", 16);
            break;
        case CHARLIT:
            strncpy(id, "CHARLIT", 16);
            break;
        case STRING:
            strncpy(id, "STRING", 16);
            break;
        case NUMBER:
            strncpy(id, "NUMBER", 16);
            break;
        case INDSEL:
            strncpy(id, "INDSEL", 16);
            break;
        case PLUSPLUS:
            strncpy(id, "PLUSPLUS", 16);
            break;
        case MINUSMINUS:
            strncpy(id, "MINUSMINUS", 16);
            break;
        case SHL:
            strncpy(id, "SHL", 16);
            break;
        case SHR:
            strncpy(id, "SHR", 16);
            break;
        case LTEQ:
            strncpy(id, "LTEQ", 16);
            break;
        case GTEQ:
            strncpy(id, "GTEQ", 16);
            break;
        case EQEQ:
            strncpy(id, "EQEQ", 16);
            break;
        case NOTEQ:
            strncpy(id, "NOTEQ", 16);
            break;
        case LOGAND:
            strncpy(id, "LOGAND", 16);
            break;
        case LOGOR:
            strncpy(id, "LOGOR", 16);
            break;
        case ELLIPSIS:
            strncpy(id, "ELLIPSIS", 16);
            break;
        case TIMESEQ:
            strncpy(id, "TIMESEQ", 16);
            break;
        case DIVEQ:
            strncpy(id, "DIVEQ", 16);
            break;
        case MODEQ:
            strncpy(id, "MODEQ", 16);
            break;
        case PLUSEQ:
            strncpy(id, "PLUSEQ", 16);
            break;
        case MINUSEQ:
            strncpy(id, "MINUSEQ", 16);
            break;
        case SHLEQ:
            strncpy(id, "SHLEQ", 16);
            break;
        case SHREQ:
            strncpy(id, "SHREQ", 16);
            break;
        case ANDEQ:
            strncpy(id, "ANDEQ", 16);
            break;
        case OREQ:
            strncpy(id, "OREQ", 16);
            break;
        case XOREQ:
            strncpy(id, "XOREQ", 16);
            break;
        case AUTO:
            strncpy(id, "AUTO", 16);
            break;
        case BREAK:
            strncpy(id, "BREAK", 16);
            break;
        case CASE:
            strncpy(id, "CASE", 16);
            break;
        case CHAR:
            strncpy(id, "CHAR", 16);
            break;
        case CONST:
            strncpy(id, "CONST", 16);
            break;
        case CONTINUE:
            strncpy(id, "CONTINUE", 16);
            break;
        case DEFAULT:
            strncpy(id, "DEFAULT", 16);
            break;
        case DO:
            strncpy(id, "DO", 16);
            break;
        case DOUBLE:
            strncpy(id, "DOUBLE", 16);
            break;
        case ELSE:
            strncpy(id, "ELSE", 16);
            break;
        case ENUM:
            strncpy(id, "ENUM", 16);
            break;
        case EXTERN:
            strncpy(id, "EXTERN", 16);
            break;
        case FLOAT:
            strncpy(id, "FLOAT", 16);
            break;
        case FOR:
            strncpy(id, "FOR", 16);
            break;
        case GOTO:
            strncpy(id, "GOTO", 16);
            break;
        case IF:
            strncpy(id, "IF", 16);
            break;
        case INLINE:
            strncpy(id, "INLINE", 16);
            break;
        case INT:
            strncpy(id, "INT", 16);
            break;
        case LONG:
            strncpy(id, "LONG", 16);
            break;
        case REGISTER:
            strncpy(id, "REGISTER", 16);
            break;
        case RESTRICT:
            strncpy(id, "RESTRICT", 16);
            break;
        case RETURN:
            strncpy(id, "RETURN", 16);
            break;
        case SHORT:
            strncpy(id, "SHORT", 16);
            break;
        case SIGNED:
            strncpy(id, "SIGNED", 16);
            break;
        case SIZEOF:
            strncpy(id, "SIZEOF", 16);
            break;
        case STATIC:
            strncpy(id, "STATIC", 16);
            break;
        case STRUCT:
            strncpy(id, "STRUCT", 16);
            break;
        case SWITCH:
            strncpy(id, "SWITCH", 16);
            break;
        case TYPEDEF:
            strncpy(id, "TYPEDEF", 16);
            break;
        case UNION:
            strncpy(id, "UNION", 16);
            break;
        case UNSIGNED:
            strncpy(id, "UNSIGNED", 16);
            break;
        case VOID:
            strncpy(id, "VOID", 16);
            break;
        case VOLATILE:
            strncpy(id, "VOLATILE", 16);
            break;
        case WHILE:
            strncpy(id, "WHILE", 16);
            break;
        case BOOL:
            strncpy(id, "BOOL", 16);
            break;
        case COMPLEX:
            strncpy(id, "COMPLEX", 16);
            break;
        case IMAGINARY:
            strncpy(id, "IMAGINARY", 16);
            break;
        default: // char code
            if(token_code < 255) {
                strncpy(id, "\0", 16); //kludge
                id[0] = (char) token_code;
            }
            else
                snprintf(id, 16, "%d", token_code);
            break;
    }
    return id;
}
