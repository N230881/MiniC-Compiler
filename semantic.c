#include <stdio.h>
#include <string.h>
#include "semantic.h"

#define MAX_SYMS 256
static char symbols[MAX_SYMS][64];
static int symCount = 0;
static int errors = 0;

static int isDeclared(const char *name) {
    for (int i = 0; i < symCount; i++)
        if (strcmp(symbols[i], name) == 0) return 1;
    return 0;
}

static void declare(const char *name, int line) {
    if (isDeclared(name)) {
        fprintf(stderr, "Semantic error at line %d: variable '%s' already declared\n", line, name);
        errors++;
        return;
    }
    strcpy(symbols[symCount++], name);
}

static void checkExpr(Node *n) {
    if (!n) return;
    switch (n->type) {
        case N_VAR:
            if (!isDeclared(n->name)) {
                fprintf(stderr, "Semantic error at line %d: variable '%s' used before declaration\n", n->line, n->name);
                errors++;
            }
            break;
        case N_BINOP:
            checkExpr(n->a);
            checkExpr(n->b);
            break;
        case N_UNOP:
            checkExpr(n->a);
            break;
        default:
            break; /* N_NUM needs no check */
    }
}

static void checkStmt(Node *n) {
    if (!n) return;
    switch (n->type) {
        case N_PROGRAM:
        case N_BLOCK:
            for (int i = 0; i < n->stmtCount; i++) checkStmt(n->stmts[i]);
            break;
        case N_DECL:
            checkExpr(n->a);
            declare(n->name, n->line);
            break;
        case N_ASSIGN:
            checkExpr(n->a);
            if (!isDeclared(n->name)) {
                fprintf(stderr, "Semantic error at line %d: variable '%s' assigned before declaration\n", n->line, n->name);
                errors++;
            }
            break;
        case N_IF:
            checkExpr(n->a);
            checkStmt(n->b);
            checkStmt(n->c);
            break;
        case N_WHILE:
            checkExpr(n->a);
            checkStmt(n->b);
            break;
        case N_FOR:
            checkStmt(n->a);
            checkExpr(n->b);
            checkStmt(n->c);
            checkStmt(n->d);
            break;
        case N_PRINT:
            checkExpr(n->a);
            break;
        default:
            break;
    }
}

int semanticCheck(Node *program) {
    symCount = 0;
    errors = 0;
    checkStmt(program);
    return errors == 0;
}
