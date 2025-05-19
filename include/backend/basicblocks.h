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

enum argmode
{
    M_LITERAL,
    M_POINTER,
    M_ARRAY
};

#define LITERALTYPE(am)\
    (M_LITERAL == am)

#define ADDRTYPE(am)\
    ((M_POINTER == am) || (M_ARRAY == am))

enum quadtypes
{
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
    Q_BRGT,
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

    enum argmode am;
    uint32_t size;

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
    int bb_num;

    struct bb_op *start;
    
    // cursor storage
    struct bb *next;
    
    //  branching if needed
    struct bb *t;        // true
    struct bb *f;    // false
};

#define LEFTOP\
    (cursor.mode == 1)

#define RIGHTOP\
    (cursor.mode == 0)

struct bb_cursor
{
    int fn_num_counter; // counts the number of functions we've encountered
    int bb_num_counter; // countds number of bb's in current function
                        //  resets upon entrance to new fcn
    int reg_count;      // next available reg

    int mode;           // 1 - we are in leftop mode
                        // 0 - we are in rightop mode
    
    struct bb *head;

    struct bb *current;
};

// src/backend/basicblocks.c

// creates an empty basic block, of the form BB.{fn_num}.{num}
struct bb *bb_create(struct bb_cursor *cursor);
void bb_op_append(struct bb_op *op, struct bb *block);
//void cursor_ingest(struct bb *block);
void cursor_ingest();
struct bb_arg *bb_gen_condexpr(ast_node *c, struct bb *root,
        struct bb *t, struct bb *f);
struct bb *bb_gen_if(ast_node *c, struct bb *root);
//struct bb_arg *bb_gen_ir(ast_node *n, struct bb *block);
struct bb_arg *bb_gen_ir(ast_node *n);

// src/backend/bb_args.c

struct bb_arg *create_arg(enum args argtype, struct bb_arg *inheritor);
struct bb_arg *generate_inheritor(struct bb_arg *src1, struct bb_arg *src2);

// src/backend/bb_ops.c

struct bb_op *bb_genop(enum quadtypes qt);
struct bb_arg *bb_op_generate_constant(ast_node *n, struct bb *block);
struct bb_arg *bb_op_generate_intconst(uint32_t num, struct bb *block);
struct bb_arg *bb_op_generate_ident(ast_node *n);

struct bb_arg *bb_op_generate_mov(struct bb_arg *src, struct bb_arg *dest, struct bb *block);
struct bb_arg *bb_op_generate_store(struct bb_arg *src, struct bb_arg *dest, struct bb *block);

struct bb_arg *bb_op_generate_mul(struct bb_arg *src1, struct bb_arg *src2, struct bb *block);
struct bb_arg *bb_op_generate_addition(struct bb_arg *l, struct bb_arg *r, struct bb *block);
struct bb_arg *bb_op_generate_div(struct bb_arg *src1, struct bb_arg *src2, struct bb *block);
struct bb_arg *bb_op_generate_mod(struct bb_arg *src1, struct bb_arg *src2, struct bb *block);
struct bb_arg *bb_op_generate_sub(struct bb_arg *src1, struct bb_arg *src2, struct bb *block);

struct bb_arg *bb_op_generate_load(struct bb_arg *src1, struct bb_arg *dest, struct bb *block);
struct bb_arg *bb_op_generate_lea(struct bb_arg *src1, struct bb_arg *dest, struct bb *block);

struct bb_arg *bb_op_generate_cmp(struct bb_arg *src1, struct bb_arg *src2, struct bb *block);
//struct bb_arg *bb_op_generate_brlt(struct bb_arg *res, struct bb *block);
//struct bb_arg *bb_op_generate_brgt(struct bb_arg *res, struct bb *block);
//struct bb_arg *bb_op_generate_breq(struct bb_arg *res, struct bb *block);
//struct bb_arg *bb_op_generate_brneq(struct bb_arg *res, struct bb *block);

struct bb_arg *bb_op_generate_declarators(ast_node *d, struct bb *block);

// src/backend/bbprint.c

void cursorprint();
void bbprint(struct bb *block);
void bbprint_op(struct bb_op *o);
char *genargstr(struct bb_arg *a);
void free_if_not_null(char *s);

#endif
