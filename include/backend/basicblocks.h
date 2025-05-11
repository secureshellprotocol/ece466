#ifndef __BASICBLOCKS_H_JR
#define __BASICBLOCKS_H_JR

#include <ast/ast.h>
#include <symtab/symtab.h>

enum args
{
    A_VAR,
    A_IMM,
    A_REG
};

enum quadtypes
{
    Q_INCOMPLETE,
    Q_NOOP,
    Q_ADD,
    Q_SUB,
    Q_MULT,
    Q_DIV,
    Q_MOD,
    Q_JUMP,
    Q_LEA,
    Q_LOAD,
    Q_MOV,
    Q_STORE,
    Q_CMP,
    Q_BRLT,
    Q_BRLE,
    Q_BRGT,
    Q_BRGE,
    Q_BREQ,
    Q_BRNEQ,
    Q_ARG,
    Q_TERM
};

/* arg types */

// variable reference from a scope
//  can be a direct reference, or a pointer to something. keep in mind!
struct bb_arg_var {
    symtab_elem *ste;
    enum scopes scope;
};

// immediate value
struct bb_arg_imm {
    unsigned long long val;
};

// register value
struct bb_arg_reg {
    unsigned long long rn;
};

struct bb_arg {
    enum args at;
    
    union
    {
        struct bb_arg_var v;
        struct bb_arg_imm i;
        struct bb_arg_reg r;
    };
};

struct bb_op {
    enum quadtypes qt;

    struct bb_arg *src1;
    struct bb_arg *src2;
    struct bb_arg *dest;

    struct bb_op *next;
};

struct bb {
    int fn_num;
    int num;

    struct bb_op *start;
};

// reframe to bb_state
struct bb_list 
{
    struct bb *head;
};

// src/backend/basicblocks.c

// creates an empty basic block, of the form BB.{fn_num}.{num}
struct bb *bb_create(int fn_num, int number);
int bb_gen_ir(symbol_scope *s, ast_node *n, struct bb block);

// src/backend/bb_args.c

struct bb_arg *create_arg(enum args argtype);

// src/backend/bb_ops.c



// src/backend/bbprint.c

void bbprint(struct bb);

#endif
