#ifndef __PARSER_ENUM_JR
#define __PARSER_ENUM_JR
// TODO deprecate include/lexer/tokens.h

#define STGCLASS    1
#define TYPESPEC    2
#define TYPEQUAL    3

enum tokens {
    // ast node types
    UNAOP=400,
    BINOP,
    TERNOP,
    TYPECAST,
    FUNCTION,
    LIST,
    POINTER,
    DECLARATION,
    SCALAR,
    ARRAY,
    VARIABLE
};
#endif
