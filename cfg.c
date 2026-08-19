#include <stdio.h>
#include <stdlib.h>
#include "cfg.h"

void printCFG(Program *p) {
    if (p->count == 0) { printf("(empty program)\n"); return; }

    /* Step 1: find "leaders" - the first instruction of every basic block.
       Standard algorithm: instruction 0, every jump TARGET (our labels),
       and every instruction right after a GOTO/IFFALSE_GOTO. */
    int *isLeader = calloc(p->count, sizeof(int));
    isLeader[0] = 1;
    for (int i = 0; i < p->count; i++) {
        Instr in = p->instrs[i];
        if (in.op == OP_LABEL) isLeader[i] = 1;
        if ((in.op == OP_GOTO || in.op == OP_IFFALSE_GOTO) && i + 1 < p->count)
            isLeader[i + 1] = 1;
    }

    int *leaders = malloc(sizeof(int) * p->count);
    int blockCount = 0;
    for (int i = 0; i < p->count; i++)
        if (isLeader[i]) leaders[blockCount++] = i;

    /* Step 2: map each label id -> which block it starts */
    int labelCap = p->labelCount > 0 ? p->labelCount : 1;
    int *labelBlock = malloc(sizeof(int) * labelCap);
    for (int i = 0; i < labelCap; i++) labelBlock[i] = -1;
    for (int b = 0; b < blockCount; b++) {
        Instr in = p->instrs[leaders[b]];
        if (in.op == OP_LABEL) labelBlock[in.label] = b;
    }

    /* Step 3: print each block and its outgoing edges */
    for (int b = 0; b < blockCount; b++) {
        int start = leaders[b];
        int end = (b + 1 < blockCount) ? leaders[b + 1] - 1 : p->count - 1;

        printf("Block B%d:\n", b);
        for (int i = start; i <= end; i++) printInstr(&p->instrs[i]);

        Instr last = p->instrs[end];
        if (last.op == OP_GOTO) {
            printf("  -> B%d\n", labelBlock[last.label]);
        } else if (last.op == OP_IFFALSE_GOTO) {
            int falseBlock = labelBlock[last.label];
            int trueBlock = (b + 1 < blockCount) ? b + 1 : -1;
            if (trueBlock != -1) printf("  -> B%d (if true)\n", trueBlock);
            printf("  -> B%d (if false)\n", falseBlock);
        } else if (b + 1 < blockCount) {
            printf("  -> B%d (fall through)\n", b + 1);
        } else {
            printf("  -> end of program\n");
        }
        printf("\n");
    }

    free(isLeader);
    free(leaders);
    free(labelBlock);
}
