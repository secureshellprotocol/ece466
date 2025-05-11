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

extern symbol_scope *current;

#define ENTER_SCOPE(scope) \
    current = scope;

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

    //printf("%d", n->op_type);

    switch(n->op_type)
    {
    case IDENT:
        {
            symbol_scope *s = current;
            while(s != NULL)
            {
                symtab_elem *e = symtab_lookup(s, n->ident.value, NS_IDENTS);
                if(e != NULL)
                {
                    printf("stab_var name=%s def @ %s:%u\n", 
                            e->key, e->file_origin, e->line_num_origin);
                    goto lookup_done;
                }
                s = s->previous;
            }
            // couldnt find it
            STDERR_F("Couldnt find ident %s in any scope!", n->ident.value);
        }
lookup_done:
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
    case COMPOUND_SCOPE:
        ENTER_SCOPE(n->cs.st);
        printf("\r");
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
        
        depth++; 
        if(n->ternop.left != NULL)
            astprint(n->ternop.left);

        JUSTIFY;
        printf("TRUE:\n");
        if(n->ternop.middle != NULL)
            astprint(n->ternop.middle);
        JUSTIFY;
        printf("ELSE:\n");
        if(n->ternop.right != NULL)
            astprint(n->ternop.right);
        depth--;
        break;
    case LIST:
        printf("\r");
        astprint(n->list.value);
        if(n->list.next != NULL)
        {
            depth--;
            astprint(n->list.next);
            depth++;
        }
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
        printf("FUNCTION\n");
        if(n->fncall.label != NULL)
        {
            //JUSTIFY;
            astprint(n->fncall.label);
        }
        if(n->fncall.arglist != NULL)
        {
            JUSTIFY;
            printf("WITH ARGUMENTS \n");
            astprint(n->fncall.arglist);
        }
        break;
    case DECLARATION:
        printf("DECLARATION\n");
        depth++;
        JUSTIFY;
        printf("STORAGE CLASS: ");
        if(n->d.stgclass == NULL)
        {
            printf("NONE\n");
        }
        else
        {
            astprint(n->d.stgclass);
        }
        JUSTIFY;
        printf("WITH DECLARATION SPECIFIERS:\n");
        astprint(n->d.decl_specs);
        JUSTIFY;
        printf("AND DECLARATORS:\n");
        astprint(n->d.declarator);
        depth--;
        break;
    case FNDEF:
        printf("FUNCTION DEFINITION\n");
        depth++;
        astprint(n->fndef.decl);
        if(n->fndef.stmt_list != NULL)
        {
            JUSTIFY;
            printf("Function Body\n");
            astprint(n->fndef.stmt_list);
        }
        depth--;
        break;
    case IF:
        printf("IF\n");
        JUSTIFY;
        printf("EXPR:\n");
        if(n->if_s.expr != NULL)
        {
            astprint(n->if_s.expr);
        }
        else printf(" EMPTY\n");

        JUSTIFY;
        printf("TRUE:\n");
        if(n->if_s.stmt != NULL)
        {
            astprint(n->if_s.stmt);
        }
        else printf(" EMPTY\n");
        
        if(n->if_s.else_stmt != NULL)
        {
            JUSTIFY;
            printf("ELSE\n");
            astprint(n->if_s.else_stmt);
        }

        break;
    case SWITCH:
        printf("SWITCH\n");
        JUSTIFY;
        printf("EXPR:\n");
        if(n->switch_s.expr != NULL)
        {
            astprint(n->switch_s.expr);
        }
        else printf(" EMPTY\n");

        JUSTIFY;
        printf("STMT:\n");
        if(n->switch_s.stmt != NULL)
        {
            astprint(n->switch_s.stmt);
        }
        else printf(" EMPTY\n");

        break;
    case WHILE:
        printf("WHILE\n");
        JUSTIFY;
        printf("EXPR:\n");
        if(n->while_s.expr != NULL)
        {
            astprint(n->while_s.expr);
        }
        else printf(" EMPTY\n");

        JUSTIFY;
        printf("STMT:\n");
        if(n->while_s.stmt != NULL)
        {
            astprint(n->while_s.stmt);
        }
        else printf(" EMPTY\n");

        break;
    case DO:
        printf("DO\n");
        JUSTIFY;
        printf("EXPR:\n");
        if(n->do_while_s.expr != NULL)
        {
            astprint(n->do_while_s.expr);
        }
        else printf(" EMPTY\n");

        JUSTIFY;
        printf("STMT:\n");
        if(n->do_while_s.stmt != NULL)
        {
            astprint(n->do_while_s.stmt);
        }
        else printf(" EMPTY\n");

        break;
    case FOR:
        printf("FOR\n");
        JUSTIFY;
        printf("INIT:\n");
        if(n->for_s.cl1 != NULL)
        {
            astprint(n->for_s.cl1);
        }
        else printf(" EMPTY\n");

        JUSTIFY;
        printf("COND:\n");
        if(n->for_s.cl2 != NULL)
        {
            astprint(n->for_s.cl2);
        }
        else printf(" EMPTY\n");

        JUSTIFY;
        printf("INC:\n");
        if(n->for_s.cl3 != NULL)
        {
            astprint(n->for_s.cl3);
        }
        else printf(" EMPTY\n");

        JUSTIFY;
        printf("BODY:\n");
        if(n->for_s.stmt != NULL)
        {
            astprint(n->for_s.stmt);
        }
        else printf(" EMPTY\n");
        
        break;
    case GOTO:
        printf("GOTO");
        if(n->goto_s.ident != NULL)
        {
            printf("(%s) ",n->goto_s.ident->ident.value);
        } else { printf("(\?\?\?)"); }

        if(symtab_lookup(current, n->goto_s.ident->ident.value, NS_LABELS) == NULL)
        {
            printf("(DEF)");
        }
        printf("\n");

        break;
    case RETURN:
        printf("RETURN");
        if(n->return_s.ret_expr != NULL)
        {
            astprint(n->return_s.ret_expr);
        }
        break;
    case LABEL:
        printf("LABEL");
        depth++;
        JUSTIFY;
        if(n->label_s.ident != NULL)
        {
            printf("(%s)", n->label_s.ident->ident.value);
        }
        printf(":\n");
        if(n->label_s.stmt != NULL)
        {
            astprint(n->label_s.stmt);
        }
        depth--;
        break;
    case CASE:
        printf("CASE\n");
        depth++;
        JUSTIFY;
        if(n->label_s.expr != NULL)
        {
            printf("EXPR:\n");
            astprint(n->label_s.expr);
        }
        else
        {
            printf("NO EXPR\n");
        }
        JUSTIFY;
        if(n->label_s.stmt != NULL)
        {
            printf("STMT:\n");
            astprint(n->label_s.stmt);
        }
        depth--;
        break;
    case DEFAULT:
        printf("DEFAULT\n");
        depth++;
        JUSTIFY;
        if(n->label_s.stmt != NULL)
        {
            printf("STATEMENT: \n");
            astprint(n->label_s.stmt);
        }
        depth--;
        break;
    // constant keywords -- 100j 
    case AUTO:
        printf("AUTO\n");
        break;
    case BREAK:
        printf("BREAK\n");
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
    case IMPLICIT_EXTERN:
        printf("EXTERN (implicit)\n");
        break;
    case FLOAT:
        printf("FLOAT\n");
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
        printf("VOID\n");
        break;
    case VOLATILE:
        printf("VOLATILE\n");
        break;
    default:
        STDERR("If I cannot bend Heaven, I shall move Hell.");
        STDERR_F("Unexpected node type %d\n", n->op_type);
    }

    depth--;
    return;
}


