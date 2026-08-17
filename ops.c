#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ops.h"

int applyBinOp(const char *op, int a, int b) {
    if (strcmp(op, "+") == 0) return a + b;
    if (strcmp(op, "-") == 0) return a - b;
    if (strcmp(op, "*") == 0) return a * b;
    if (strcmp(op, "/") == 0) {
        if (b == 0) { fprintf(stderr, "Runtime error: division by zero\n"); exit(1); }
        return a / b;
    }
    if (strcmp(op, "%") == 0) {
        if (b == 0) { fprintf(stderr, "Runtime error: mod by zero\n"); exit(1); }
        return a % b;
    }
    if (strcmp(op, "<") == 0) return a < b;
    if (strcmp(op, ">") == 0) return a > b;
    if (strcmp(op, "<=") == 0) return a <= b;
    if (strcmp(op, ">=") == 0) return a >= b;
    if (strcmp(op, "==") == 0) return a == b;
    if (strcmp(op, "!=") == 0) return a != b;
    if (strcmp(op, "&&") == 0) return a && b;
    if (strcmp(op, "||") == 0) return a || b;
    fprintf(stderr, "Error: unknown binary operator %s\n", op);
    exit(1);
}

int applyUnOp(const char *op, int a) {
    if (strcmp(op, "-") == 0) return -a;
    if (strcmp(op, "!") == 0) return !a;
    fprintf(stderr, "Error: unknown unary operator %s\n", op);
    exit(1);
}
