#ifndef AST_H
#define AST_H

typedef enum {
    N_NUM, N_VAR, N_BINOP, N_UNOP,
    N_ASSIGN, N_DECL, N_IF, N_WHILE, N_FOR,
    N_PRINT, N_BLOCK, N_PROGRAM
} NodeType;

typedef struct Node {
    NodeType type;
    char op[3];
    char name[64];
    int ival;
    struct Node *a, *b, *c, *d;
    struct Node **stmts;
    int stmtCount;
    int line;
} Node;

Node* newNode(NodeType type);
void addStmt(Node *block, Node *stmt);
void printAST(Node *n, int depth);

#endif
