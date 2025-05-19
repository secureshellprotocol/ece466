#include <stdlib.h>

#include <james_utils.h>
#include <ast/ast.h>
#include <symtab/symtab.h>
#include <backend/basicblocks.h>

extern struct bb_cursor cursor;

struct bb_arg *create_arg(enum args argtype, struct bb_arg *inheritor)
{
    struct bb_arg *a = calloc(1, sizeof(struct bb_arg));

    if(inheritor != NULL)
    {
        a->size = inheritor->size;
        a->am = inheritor->am;
    }
    else
    {
        a->size = sizeof(int);
        a->am = M_LITERAL;
    }

    if(a == NULL)
    {
        ERROR("Failed to allocate a basicblock argument!");
    }

    a->at = argtype;

    switch(a->at)
    {
        case A_REG:
            a->r.rn = cursor.reg_count;
            cursor.reg_count++;
            break;
        case A_VAR: case A_IMM:
            break;
        default:
            break;
    }

    return a;
}

// each inheritor basically tells us whats contained in our
// register/var/whatever
// this is for following registers and being able to tell if what we have inside
// is either a literal (int, char, etc), or an address (array, pointer)
// src2 can be optionally null
struct bb_arg *generate_inheritor(struct bb_arg *src1, struct bb_arg *src2)
{
    struct bb_arg *a = create_arg(A_IMM, NULL); // A_IMM doesnt matter
    if(src1 == NULL)
    {
        STDERR("source 1 operand must not be null!");
        return NULL;
    }
    if(src2 == NULL)
        src2 = create_arg(A_IMM, NULL);
    
    if(ADDRTYPE(src1->am) || ADDRTYPE(src2->am))
    {
        a->size = sizeof(int *);
        a->am = M_POINTER;

        // The array subscript is always going to be an integer. so we should
        // never see the siutaion of two M_ARRAY. Gonna make an error here if
        // that is the case.
        if(src1->am == src2->am)
        {
            if(src1->am == M_ARRAY)
            {
                STDERR("WARN: Both src's are in Array mode! src1 takes precedence");
            }

            // both are pointers
            a->am = M_ARRAY;
            a->size = src1->size;
            return a;
        }
        else
        {
            if(src1->am == M_ARRAY)
            {
                a->am = M_ARRAY;
                a->size = src1->size;
            }
            if(src2->am == M_ARRAY)
            {
                a->am = M_ARRAY;
                a->size = src2->size;
            }
            return a;
        }
    }
    return NULL;
}
