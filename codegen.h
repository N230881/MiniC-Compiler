#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"

typedef enum {
    OP_LOADCONST,     /* dst = ival */
    OP_LOADVAR,       /* dst = varname */
    OP_STOREVAR,      /* varname = src1 */
    OP_BINOP,         /* dst = src1 op src2 */
    OP_UNOP,          /* dst = op src1 */
    OP_PRINT,         /* print src1 */
    OP_LABEL,         /* label: */
    OP_GOTO,          /* goto label */
    OP_IFFALSE_GOTO   /* if_false src1 goto label */
} OpCode;

typedef struct {
    OpCode op;
    int dst, src1, src2;   /* temp register indices, -1 if unused */
    char varname[64];
    int ival;
    char opstr[3];
    int label;
} Instr;

typedef struct {
    Instr *instrs;
    int count;
    int tempCount;
    int labelCount;
} Program;

Program generateCode(Node *root);
void printInstr(const Instr *in);   /* prints one instruction line (shared with cfg.c) */
void printTAC(Program *p);
void freeProgram(Program *p);

#endif
