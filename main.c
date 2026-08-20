#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "optimizer.h"
#include "cfg.h"
#include "vm.h"

static char* readFile(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Error: cannot open file '%s'\n", path); exit(1); }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(size + 1);
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source-file.mc>\n", argv[0]);
        return 1;
    }

    char *source = readFile(argv[1]);

    printf("===TOKENS===\n");
    TokenList tokens = lex(source);
    printTokens(&tokens);

    printf("===AST===\n");
    Node *program = parseProgram(&tokens);
    printAST(program, 0);

    printf("===SEMANTIC===\n");
    int ok = semanticCheck(program);
    printf(ok ? "No semantic errors found.\n" : "Semantic errors found (see above).\n");

    printf("===IR===\n");
    Program code = generateCode(program);
    printTAC(&code);

    printf("===OPTIMIZER===\n");
    OptStats stats = optimizeProgram(&code);
    printf("Constant folding      : simplified %d expression(s)\n", stats.foldedCount);
    printf("Dead code elimination : removed %d unused instruction(s)\n", stats.eliminatedCount);

    printf("===OPTIMIZED_IR===\n");
    printTAC(&code);

    printf("===CFG===\n");
    printCFG(&code);

    printf("===OUTPUT===\n");
    if (ok) {
        runProgram(&code);
    } else {
        printf("(skipped: fix semantic errors first)\n");
    }
    printf("===END===\n");

    free(source);
    freeTokenList(&tokens);
    freeProgram(&code);
    return 0;
}
