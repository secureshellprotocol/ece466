#ifndef __PARSER_ENUM_JR
#define __PARSER_ENUM_JR
// TODO deprecate include/lexer/tokens.h

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
    ARRAY
};
#endif
