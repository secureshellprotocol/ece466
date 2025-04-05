#include <stdio.h>
#include <stdlib.h>

#include <ast/ast.h>
#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>

// macro for `astprint(ast_node *)`
//  inserts a number of spaces equal to the depth of the tree.
//  think of justifying your indents during grade school -- good times!
#define JUSTIFY \
    printf("%*c", depth+1, ' ');

// selecting some root node, start to print
void astprint(ast_node *n)
{
    static int depth;
    ++depth;

    JUSTIFY
    switch(n->op_type)
    {
    case IDENT:
        printf("IDENT %s\n", n->ident.value);
        break;
    case NUMBER:
        printf("NUM (numtype=");
        if(LEX_IS_FLOATING(n->num.tags))
            printf("float) %Lg\n", n->num.fval);
        else printf("int) %llu\n", n->num.ival);
        break;
    case CHARLIT:
        printf("CHARLIT ");
        chardecode(n->char_array.value[0]);
        printf("\n");
        break;
    case STRING:
        printf("STRING ");
        for(int i = 0; i < n->char_array.s_len; i++)
        {
            chardecode(n->char_array.value[i]);
        }
        printf("\n");
        break;
    case UNAOP:
        {
            char *tok_id = get_token_id(n->unaop.token);
            printf("UNARY OP %s\n", tok_id); 
        }
        if(n->unaop.expression != NULL)
            astprint(n->unaop.expression);

        break;
    case BINOP:
        {
            char *tok_id = get_token_id(n->binop.token);
            printf("BINARY OP %s\n", tok_id); 
        }
        if(n->binop.left != NULL)
            astprint(n->binop.left);
        if(n->binop.right != NULL)
            astprint(n->binop.right);

        break;
    case TERNOP:
        printf("TERNARY OP \n"); 
        
        if(n->ternop.left != NULL)
            astprint(n->ternop.left);
         
        if(n->ternop.middle != NULL)
            astprint(n->ternop.middle);

        if(n->ternop.right != NULL)
            astprint(n->ternop.right);
        break;
    case ARRAY:
        printf("ARRAY");
        if(n->array.to != NULL)
        {
            printf(" OF TYPE:");
            astprint(n->array.to);
            printf(" AND SIZE: ");
            astprint(n->array.size);
        }
        else 
        {
            printf(" INCOMPLETE");
        }
        break;
    case POINTER:
        printf("POINTER");
        if(n->ptr.to != NULL)
        {
            astprint(n->ptr.to);
        }
        break;
    case FUNCTION:
        printf("FUNCTION");
        astprint(n->func.label);
        if(n->func.decl_specs != NULL)
        {
            printf("WITH DECLARATION SPECIFIERS \n");
            astprint(n->func.decl_specs);
        }
        if(n->func.params_list != NULL)
        {
            printf("WITH PARAMETERS \n");
            astprint(n->func.params_list);
        }
        break;
    case DECLARATION:
        printf("DECLARATION");
        if(n->decl.decl_specs != NULL)
        {
            printf(" of SPECS:\n");
            astprint(n->decl.decl_specs);
        }
        if(n->decl.decl_list != NULL)
        {
            printf("declaring the following IDENTS:\n");
            astprint(n->decl.decl_list);
        }
    case LIST:
        printf("LIST ITEM");
        astprint(n->list.value);
        if(n->list.next != NULL)
        {
            depth--;
            astprint(n->list.next);
            depth++;
        }
        break;

    // constant keywords -- 100j 
    case AUTO:
        printf("AUTO\n");
        break;
    case BREAK:
        printf("BREAK\n");
        break;
    case CASE:
        printf("CASE\n");
        break;
    case CHAR:
        printf("CHAR\n");
        break;
    case CONST:
        printf("CONST\n");
        break;
    case CONTINUE:
        printf("CONTINUE\n");
        break;
    case DEFAULT:
        printf("DEFAULT\n");
        break;
    case DO:
        printf("DO\n");
        break;
    case DOUBLE:
        printf("DOUBLE\n");
        break;
    case ELSE:
        printf("ELSE\n");
        break;
    case ENUM:
        printf("ENUM\n");
        break;
    case EXTERN:
        printf("EXTERN\n");
        break;
    case FLOAT:
        printf("FLOAT\n");
        break;
    case FOR:
        printf("FOR\n");
        break;
    case GOTO:
        printf("GOTO\n");
        break;
    case IF:
        printf("IF\n");
        break;
    case INLINE:
        printf("INLINE\n");
        break;
    case INT:
        printf("INT\n");
        break;
    case LONG:
        printf("LONG\n");
        break;
    case REGISTER:
        printf("REGISTER\n");
        break;
    case RESTRICT:
        printf("RESTRICT\n");
        break;
    case RETURN:
        printf("RETURN\n");
        break;
    case SHORT:
        printf("SHORT\n");
        break;
    case SIGNED:
        printf("SIGNED\n");
        break;
    case SIZEOF:
        printf("SIZEOF\n");
        break;
    case STATIC:
        printf("STATIC\n");
        break;
    case STRUCT:
        printf("STRUCT\n");
        break;
    case SWITCH:
        printf("SWITCH\n");
        break;
    case TYPEDEF:
        printf("TYPEDEF\n");
        break;
    case UNION:
        printf("UNION\n");
        break;
    case UNSIGNED:
        printf("UNSIGNED\n");
        break;
    case VOID:
        printf("VOUD\n");
        break;
    case VOLATILE:
        printf("VOLATILE\n");
        break;
    case WHILE:
        printf("WHEEL\n");
        break;
    default:
        fprintf(stderr, "astprint: If I cannot bend Heaven, I shall move Hell.\n\
            astprint: unexpected node type %d\n", n->op_type);
    }

    depth--;
    return;
}
