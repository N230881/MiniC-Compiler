#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "codegen.h"

typedef struct {
    int foldedCount;       /* how many expressions were constant-folded */
    int eliminatedCount;   /* how many dead instructions were removed */
} OptStats;

/* Optimizes the IR (Program) in place: constant folding, then dead-code
   elimination (repeated to a fixed point). Returns stats about what changed. */
OptStats optimizeProgram(Program *p);

#endif
