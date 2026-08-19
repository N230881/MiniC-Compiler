#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "optimizer.h"
#include "ops.h"

/* ---------- Pass 1: Constant Folding ----------
   Scans the IR left to right. Whenever both operands of a BINOP/UNOP are
   known compile-time constants (because they came from a LOADCONST, or
   from a previous fold), compute the result now and rewrite the
   instruction as a single LOADCONST. This mirrors what real compilers
   (GCC/LLVM -O1) do for expressions like `2 + 3 * 4`. */
static Instr* constantFold(Program *p, int *outCount, int *foldedCount) {
    int tcount = p->tempCount > 0 ? p->tempCount : 1;
    int *known = calloc(tcount, sizeof(int));
    int *value = calloc(tcount, sizeof(int));

    Instr *out = malloc(sizeof(Instr) * p->count);
    int n = 0;

    for (int i = 0; i < p->count; i++) {
        Instr in = p->instrs[i];

        if (in.op == OP_LOADCONST) {
            known[in.dst] = 1;
            value[in.dst] = in.ival;
            out[n++] = in;

        } else if (in.op == OP_LOADVAR) {
            known[in.dst] = 0;
            out[n++] = in;

        } else if (in.op == OP_BINOP && known[in.src1] && known[in.src2]) {
            Instr ni = {0};
            ni.op = OP_LOADCONST;
            ni.dst = in.dst;
            ni.ival = applyBinOp(in.opstr, value[in.src1], value[in.src2]);
            known[in.dst] = 1;
            value[in.dst] = ni.ival;
            out[n++] = ni;
            (*foldedCount)++;

        } else if (in.op == OP_UNOP && known[in.src1]) {
            Instr ni = {0};
            ni.op = OP_LOADCONST;
            ni.dst = in.dst;
            ni.ival = applyUnOp(in.opstr, value[in.src1]);
            known[in.dst] = 1;
            value[in.dst] = ni.ival;
            out[n++] = ni;
            (*foldedCount)++;

        } else {
            if (in.op == OP_BINOP || in.op == OP_UNOP) known[in.dst] = 0;
            out[n++] = in;
        }
    }

    free(known);
    free(value);
    *outCount = n;
    return out;
}

/* ---------- Pass 2: Dead Code Elimination ----------
   An instruction that computes into a temp register (LOADCONST, LOADVAR,
   BINOP, UNOP) is "dead" if that temp is never read by anything else
   (no side effects like print/store/branch depend on it). We remove dead
   instructions and repeat until nothing more can be removed, since
   removing one dead instruction can make another one dead too. */
static Instr* deadCodeEliminate(Instr *in, int count, int tempCount, int *outCount, int *eliminatedCount) {
    Instr *cur = in;
    int curN = count;

    while (1) {
        int *used = calloc(tempCount > 0 ? tempCount : 1, sizeof(int));
        for (int i = 0; i < curN; i++) {
            Instr ins = cur[i];
            if (ins.op == OP_STOREVAR || ins.op == OP_PRINT || ins.op == OP_IFFALSE_GOTO)
                used[ins.src1] = 1;
            if (ins.op == OP_BINOP) { used[ins.src1] = 1; used[ins.src2] = 1; }
            if (ins.op == OP_UNOP) used[ins.src1] = 1;
        }

        Instr *next = malloc(sizeof(Instr) * (curN > 0 ? curN : 1));
        int nextN = 0;
        int removedThisPass = 0;

        for (int i = 0; i < curN; i++) {
            Instr ins = cur[i];
            int hasResult = (ins.op == OP_LOADCONST || ins.op == OP_LOADVAR ||
                              ins.op == OP_BINOP || ins.op == OP_UNOP);
            if (hasResult && !used[ins.dst]) {
                removedThisPass++;
                (*eliminatedCount)++;
                continue; /* drop it */
            }
            next[nextN++] = ins;
        }

        free(used);
        free(cur);
        cur = next;
        curN = nextN;

        if (removedThisPass == 0) break; /* fixed point reached */
    }

    *outCount = curN;
    return cur;
}

OptStats optimizeProgram(Program *p) {
    OptStats stats = {0, 0};

    int foldedCount = 0;
    int n1 = 0;
    Instr *folded = constantFold(p, &n1, &foldedCount);

    int eliminatedCount = 0;
    int n2 = 0;
    Instr *final = deadCodeEliminate(folded, n1, p->tempCount, &n2, &eliminatedCount);

    free(p->instrs);
    p->instrs = final;
    p->count = n2;

    stats.foldedCount = foldedCount;
    stats.eliminatedCount = eliminatedCount;
    return stats;
}
