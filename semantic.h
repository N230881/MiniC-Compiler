#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "ast.h"

/* Returns 1 if the program is semantically valid, 0 if errors were found
   (errors are printed to stderr). */
int semanticCheck(Node *program);

#endif
