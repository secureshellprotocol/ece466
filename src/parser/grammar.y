
%token NUM IDENT CHARLIT STRING

%%

primary-expression:
    IDENT
    | NUM
    | STRING
    | '(' expression ')'

postfix-expression:
    primary-expression
    | postfix-expression '[' expression ']'
    | postfix-expression '(' argument-expression-list ')'
    | postfix-expression '(' ')'
    | postfix-expression '.' IDENT
    | postfix-expression INDSEL IDENT
    | postfix-expression PLUSPLUS
    | postfix-expression MINUSMINUS

argument-expression-list:
    assignment-expression
    | argument-expression-list ',' assignment-expression



%%
