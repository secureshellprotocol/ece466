#include <stdio.h>
#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <symtab/symtab.h>
#include <backend/basicblocks.h>

extern struct bb_cursor cursor;

void cursorprint()
{
    struct bb *block = cursor.head;
    
    STDERR("Printing cursor");

    if(block == NULL)
    {
        STDERR("No blocks ingested!");
        return;
    }

    while(block != NULL)
    {
        bbprint(block);
        block = block->next;
    }

    return;
}

void bbprint(struct bb *block)
{
    if(block == NULL)
    {
        STDERR("NULL BLOCK");
        return;
    }

    if(block->start == NULL)
    {
        STDERR("Empty block");
    }

    printf(".BB.%d.%d\n", block->fn_num, block->bb_num);
    struct bb_op *o = block->start;
    while(o != NULL)
    {
        bbprint_op(o);
        o = o->next;
    }
    
    return;
}

void bbprint_op(struct bb_op *o)
{
    if(o == NULL)
    {
        STDERR("Empty op");
        return;
    }

    char *src1_s, *src2_s, *dest_s;
    
    src1_s = genargstr(o->src1);
    src2_s = genargstr(o->src2);
    dest_s = genargstr(o->dest);
    
    printf("\t");

    switch(o->qt)
    {
        case Q_MOV:
            printf("%s\t = MOV %s", dest_s, src1_s);
            break;
        case Q_ADD:
            printf("%s\t = ADD %s, %s", dest_s, src1_s, src2_s);
            break;
        case Q_LOAD:
            printf("%s\t = LOAD %s", dest_s, src1_s);
            break;
        case Q_LEA:
            printf("%s\t = LEA %s", dest_s, src1_s);
            break;
        case Q_MULT:
            printf("%s\t = MULT %s, %s", dest_s, src1_s, src2_s);
            break;
        case Q_STORE:
            printf("\t   STORE %s, %s", src1_s, dest_s);
            break;
        default:
            STDERR_F("WTF??? OP: %d", o->qt);
    }
    
    printf("\n");

    free_if_not_null(src1_s);
    free_if_not_null(src2_s);
    free_if_not_null(dest_s);

    return;
}

char *genargstr(struct bb_arg *a)
{
    char* str = calloc(128,  sizeof(char)); 

    if(a == NULL)
    {
        //STDERR("Null argument provided");
        strncpy(str, "<null>", 16);
        return str;
    }

    switch(a->at)
    {
        case A_IMM:


            snprintf(str, 128, "%lld", a->i.val);
            break;
        case A_REG:
            snprintf(str, 128, "%%%lld", a->r.rn);
            break;
        case A_VAR:
            strncpy(str, a->v.ste->key, 16);
            break;
        default:
            STDERR_F("Unhandled arg encountered while generating output for %d",
                     a->at);
            strncpy(str, "<unhandled>", 128);
            break;
    }

    return str;
}

//void argmodeprint(struct bb_arg *a)
//{
//    switch(a->am)
//    {
//        case 
//    }
//}

void free_if_not_null(char *s)
{
    if(s != NULL)
    {
        free(s);
    }
    return;
}
