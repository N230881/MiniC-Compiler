#ifndef CFG_H
#define CFG_H
#include "codegen.h"

/* Splits the IR into basic blocks (using the standard "leaders" algorithm)
   and prints each block plus its successor edges - i.e. the program's
   Control Flow Graph. */
void printCFG(Program *p);

#endif
