#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "ops.h"

#define MAX_TEMPS 4096
#define MAX_VARS 256

typedef struct { char name[64]; int val; } Var;

static int findVar(Var *vars, int count, const char *name) {
    for (int i = 0; i < count; i++)
        if (strcmp(vars[i].name, name) == 0) return i;
    return -1;
}

void runProgram(Program *p) {
    static int temps[MAX_TEMPS];
    static Var vars[MAX_VARS];
    int varCount = 0;

    /* map label id -> instruction index */
    int *labelPos = calloc(p->labelCount, sizeof(int));
    for (int i = 0; i < p->count; i++)
        if (p->instrs[i].op == OP_LABEL) labelPos[p->instrs[i].label] = i;

    int pc = 0;
    while (pc < p->count) {
        Instr in = p->instrs[pc];
        switch (in.op) {
            case OP_LOADCONST:
                temps[in.dst] = in.ival;
                break;
            case OP_LOADVAR: {
                int idx = findVar(vars, varCount, in.varname);
                temps[in.dst] = (idx == -1) ? 0 : vars[idx].val;
                break;
            }
            case OP_STOREVAR: {
                int idx = findVar(vars, varCount, in.varname);
                if (idx == -1) { idx = varCount++; strcpy(vars[idx].name, in.varname); }
                vars[idx].val = temps[in.src1];
                break;
            }
            case OP_BINOP:
                temps[in.dst] = applyBinOp(in.opstr, temps[in.src1], temps[in.src2]);
                break;
            case OP_UNOP:
                temps[in.dst] = applyUnOp(in.opstr, temps[in.src1]);
                break;
            case OP_PRINT:
                printf("%d\n", temps[in.src1]);
                break;
            case OP_LABEL:
                break;
            case OP_GOTO:
                pc = labelPos[in.label];
                continue;
            case OP_IFFALSE_GOTO:
                if (!temps[in.src1]) { pc = labelPos[in.label]; continue; }
                break;
        }
        pc++;
    }
    free(labelPos);
}
