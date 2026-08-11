#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node* newNode(NodeType type) {
    Node *n = calloc(1, sizeof(Node));
    n->type = type;
    return n;
}

void addStmt(Node *block, Node *stmt) {
    block->stmts = realloc(block->stmts, sizeof(Node*) * (block->stmtCount + 1));
    block->stmts[block->stmtCount++] = stmt;
}

static void indent(int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
}

void printAST(Node *n, int depth) {
    if (!n) return;
    indent(depth);
    switch (n->type) {
        case N_PROGRAM:
            printf("Program\n");
            for (int i = 0; i < n->stmtCount; i++) printAST(n->stmts[i], depth + 1);
            break;
        case N_BLOCK:
            printf("Block\n");
            for (int i = 0; i < n->stmtCount; i++) printAST(n->stmts[i], depth + 1);
            break;
        case N_DECL:
            printf("Decl int %s\n", n->name);
            if (n->a) printAST(n->a, depth + 1);
            break;
        case N_ASSIGN:
            printf("Assign %s =\n", n->name);
            printAST(n->a, depth + 1);
            break;
        case N_IF:
            printf("If\n");
            indent(depth + 1); printf("cond:\n"); printAST(n->a, depth + 2);
            indent(depth + 1); printf("then:\n"); printAST(n->b, depth + 2);
            if (n->c) { indent(depth + 1); printf("else:\n"); printAST(n->c, depth + 2); }
            break;
        case N_WHILE:
            printf("While\n");
            indent(depth + 1); printf("cond:\n"); printAST(n->a, depth + 2);
            indent(depth + 1); printf("body:\n"); printAST(n->b, depth + 2);
            break;
        case N_FOR:
            printf("For\n");
            indent(depth + 1); printf("init:\n"); printAST(n->a, depth + 2);
            indent(depth + 1); printf("cond:\n"); printAST(n->b, depth + 2);
            indent(depth + 1); printf("update:\n"); printAST(n->c, depth + 2);
            indent(depth + 1); printf("body:\n"); printAST(n->d, depth + 2);
            break;
        case N_PRINT:
            printf("Print\n");
            printAST(n->a, depth + 1);
            break;
        case N_BINOP:
            printf("BinOp '%s'\n", n->op);
            printAST(n->a, depth + 1);
            printAST(n->b, depth + 1);
            break;
        case N_UNOP:
            printf("UnOp '%s'\n", n->op);
            printAST(n->a, depth + 1);
            break;
        case N_VAR:
            printf("Var %s\n", n->name);
            break;
        case N_NUM:
            printf("Num %d\n", n->ival);
            break;
    }
}
