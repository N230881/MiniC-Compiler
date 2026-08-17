#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

static Program prog;
static int cap;

static void emit(Instr in) {
    if (prog.count >= cap) {
        cap *= 2;
        prog.instrs = realloc(prog.instrs, sizeof(Instr) * cap);
    }
    prog.instrs[prog.count++] = in;
}

static int newTemp(void) { return prog.tempCount++; }
static int newLabel(void) { return prog.labelCount++; }

static int genExpr(Node *n) {
    Instr in = {0};
    switch (n->type) {
        case N_NUM: {
            int t = newTemp();
            in.op = OP_LOADCONST; in.dst = t; in.ival = n->ival;
            emit(in);
            return t;
        }
        case N_VAR: {
            int t = newTemp();
            in.op = OP_LOADVAR; in.dst = t; strcpy(in.varname, n->name);
            emit(in);
            return t;
        }
        case N_BINOP: {
            int a = genExpr(n->a);
            int b = genExpr(n->b);
            int t = newTemp();
            in.op = OP_BINOP; in.dst = t; in.src1 = a; in.src2 = b; strcpy(in.opstr, n->op);
            emit(in);
            return t;
        }
        case N_UNOP: {
            int a = genExpr(n->a);
            int t = newTemp();
            in.op = OP_UNOP; in.dst = t; in.src1 = a; strcpy(in.opstr, n->op);
            emit(in);
            return t;
        }
        default:
            fprintf(stderr, "codegen: unexpected expression node\n");
            exit(1);
    }
}

static void genStmt(Node *n) {
    if (!n) return;
    Instr in = {0};
    switch (n->type) {
        case N_PROGRAM:
        case N_BLOCK:
            for (int i = 0; i < n->stmtCount; i++) genStmt(n->stmts[i]);
            break;

        case N_DECL: {
            int t = n->a ? genExpr(n->a) : -1;
            if (t == -1) {
                t = newTemp();
                in.op = OP_LOADCONST; in.dst = t; in.ival = 0;
                emit(in);
                in = (Instr){0};
            }
            in.op = OP_STOREVAR; in.src1 = t; strcpy(in.varname, n->name);
            emit(in);
            break;
        }

        case N_ASSIGN: {
            int t = genExpr(n->a);
            in.op = OP_STOREVAR; in.src1 = t; strcpy(in.varname, n->name);
            emit(in);
            break;
        }

        case N_IF: {
            int Lelse = newLabel();
            int Lend = newLabel();
            int c = genExpr(n->a);
            in = (Instr){0}; in.op = OP_IFFALSE_GOTO; in.src1 = c; in.label = Lelse; emit(in);
            genStmt(n->b);
            in = (Instr){0}; in.op = OP_GOTO; in.label = Lend; emit(in);
            in = (Instr){0}; in.op = OP_LABEL; in.label = Lelse; emit(in);
            if (n->c) genStmt(n->c);
            in = (Instr){0}; in.op = OP_LABEL; in.label = Lend; emit(in);
            break;
        }

        case N_WHILE: {
            int Lstart = newLabel();
            int Lend = newLabel();
            in = (Instr){0}; in.op = OP_LABEL; in.label = Lstart; emit(in);
            int c = genExpr(n->a);
            in = (Instr){0}; in.op = OP_IFFALSE_GOTO; in.src1 = c; in.label = Lend; emit(in);
            genStmt(n->b);
            in = (Instr){0}; in.op = OP_GOTO; in.label = Lstart; emit(in);
            in = (Instr){0}; in.op = OP_LABEL; in.label = Lend; emit(in);
            break;
        }

        case N_FOR: {
            genStmt(n->a); /* init */
            int Lstart = newLabel();
            int Lend = newLabel();
            in = (Instr){0}; in.op = OP_LABEL; in.label = Lstart; emit(in);
            int c = genExpr(n->b);
            in = (Instr){0}; in.op = OP_IFFALSE_GOTO; in.src1 = c; in.label = Lend; emit(in);
            genStmt(n->d);   /* body */
            genStmt(n->c);   /* update */
            in = (Instr){0}; in.op = OP_GOTO; in.label = Lstart; emit(in);
            in = (Instr){0}; in.op = OP_LABEL; in.label = Lend; emit(in);
            break;
        }

        case N_PRINT: {
            int t = genExpr(n->a);
            in.op = OP_PRINT; in.src1 = t;
            emit(in);
            break;
        }

        default:
            fprintf(stderr, "codegen: unexpected statement node\n");
            exit(1);
    }
}

Program generateCode(Node *root) {
    prog.instrs = NULL;
    prog.count = 0;
    prog.tempCount = 0;
    prog.labelCount = 0;
    cap = 64;
    prog.instrs = malloc(sizeof(Instr) * cap);
    genStmt(root);
    return prog;
}

void printInstr(const Instr *in) {
    switch (in->op) {
        case OP_LOADCONST: printf("  t%d = %d\n", in->dst, in->ival); break;
        case OP_LOADVAR:   printf("  t%d = %s\n", in->dst, in->varname); break;
        case OP_STOREVAR:  printf("  %s = t%d\n", in->varname, in->src1); break;
        case OP_BINOP:     printf("  t%d = t%d %s t%d\n", in->dst, in->src1, in->opstr, in->src2); break;
        case OP_UNOP:      printf("  t%d = %s t%d\n", in->dst, in->opstr, in->src1); break;
        case OP_PRINT:     printf("  print t%d\n", in->src1); break;
        case OP_LABEL:     printf("L%d:\n", in->label); break;
        case OP_GOTO:      printf("  goto L%d\n", in->label); break;
        case OP_IFFALSE_GOTO: printf("  if_false t%d goto L%d\n", in->src1, in->label); break;
    }
}

void printTAC(Program *p) {
    for (int i = 0; i < p->count; i++) printInstr(&p->instrs[i]);
}

void freeProgram(Program *p) {
    free(p->instrs);
    p->instrs = NULL;
}
