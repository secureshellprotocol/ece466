#include <stdio.h>
#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <lexer/lex_utils.h>
#include <parser/grammar.tab.h>
#include <parser/op.h>

// macro for `astprint(ast_node *)`
//  inserts a number of spaces equal to the depth of the tree.
//  think of justifying your indents during grade school -- good times!
#define JUSTIFY \
    printf("%*c", depth+1, ' ')

// selecting some root node, start to print
void astprint(ast_node *n)
{
    if(n == NULL)
    {
        STDERR("Given NULL node!");
        return;
    }

    static int depth;
    ++depth;

    JUSTIFY;
    
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
    case VARIABLE:
        printf("VARIABLE");
        JUSTIFY;
        astprint(n->var.i);
        printf(" of stgclass ");
        if(n->var.stgclass != NULL)
            astprint(n->var.stgclass);
        else
            printf(" NONE\n");
        printf(" and declspecs:\n");
        astprint(n->var.attr_list);
        break;
    case ARRAY:
        printf("ARRAY");
        JUSTIFY;
        printf(" SIZE: ");
        if(n->array.size != NULL)
            astprint(n->array.size);
        else
            printf("INCOMPLETE\n");
        break;
    case POINTER:
        printf("POINTER\n");
        break;
    case FUNCTION:
        printf("FUNCTION");
        if(n->func.label != NULL)
        {
            astprint(n->func.label);
        }
        if(n->func.decl_specs != NULL)
        {
            JUSTIFY;
            printf("WITH DECLARATION SPECIFIERS \n");
            astprint(n->func.decl_specs);
        }
        if(n->func.params_list != NULL)
        {
            JUSTIFY;
            printf("WITH PARAMETERS \n");
            astprint(n->func.params_list);
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
        STDERR("If I cannot bend Heaven, I shall move Hell.");
        STDERR_F("Unexpected node type %d\n", n->op_type);
    }

    depth--;
    return;
}


