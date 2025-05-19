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
        bbprint_op(o, block);
        o = o->next;
    }
    
    return;
}

void bbprint_op(struct bb_op *o, struct bb *block)
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
        case Q_SUB:
            printf("%s\t = SUB %s, %s", dest_s, src1_s, src2_s);
            break;
        case Q_MULT:
            printf("%s\t = MULT %s, %s", dest_s, src1_s, src2_s);
            break;
        case Q_DIV:
            printf("%s\t = DIV %s, %s", dest_s, src1_s, src2_s);
            break;
        case Q_MOD:
            printf("%s\t = MOD %s, %s", dest_s, src1_s, src2_s);
            break;
        case Q_LOAD:
            printf("%s\t = LOAD %s", dest_s, src1_s);
            break;
        case Q_LEA:
            printf("%s\t = LEA %s", dest_s, src1_s);
            break;
        case Q_STORE:
            printf("\t   STORE %s, %s", src1_s, dest_s);
            break;
        case Q_CMP:
            printf("\t   CMP %s, %s", src1_s, src2_s);
            break;
        case Q_BRLT:
            printf("\t   BRLT BB.%d.%d, BB.%d.%d", 
                    block->t->fn_num, block->t->bb_num,
                    block->f->fn_num, block->f->bb_num);
            break;
        case Q_BRGT:
            printf("\t   BRGT BB.%d.%d, BB.%d.%d", 
                    block->t->fn_num, block->t->bb_num,
                    block->f->fn_num, block->f->bb_num);
            break;
        case Q_BREQ:
            printf("\t   BREQ BB.%d.%d, BB.%d.%d", 
                    block->t->fn_num, block->t->bb_num,
                    block->f->fn_num, block->f->bb_num);
            break;
        case Q_BRNEQ:
            printf("\t   BRNEQ BB.%d.%d, BB.%d.%d", 
                    block->t->fn_num, block->t->bb_num,
                    block->f->fn_num, block->f->bb_num);
            break;
        case Q_JUMP:
            printf("\t   JUMP BB.%d.%d",
                    block->t->fn_num, block->t->bb_num);
            break;
        case Q_BREAK:
            printf("\t   JUMP BB.%d.%d", 
                    block->f->fn_num, block->f->bb_num);
            break;
        case Q_RETURN:
            printf("\t   RETURN %s", src1_s);
            break;
        case Q_ARG:
            printf("\t   ARG %s, %s", src1_s, src2_s);
            break;
        case Q_CALL:
            printf("%s\t = CALL %s, %s", dest_s, src1_s, src2_s);
            break;
        default:
            printf("WTF??? OP: %d", o->qt);
            break;
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
