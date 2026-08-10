#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static void push(TokenList *tl, int *cap, Token t) {
    if (tl->count >= *cap) {
        *cap *= 2;
        tl->tokens = realloc(tl->tokens, sizeof(Token) * (*cap));
    }
    tl->tokens[tl->count++] = t;
}

static int keywordType(const char *s) {
    if (strcmp(s, "if") == 0) return TOK_IF;
    if (strcmp(s, "else") == 0) return TOK_ELSE;
    if (strcmp(s, "while") == 0) return TOK_WHILE;
    if (strcmp(s, "for") == 0) return TOK_FOR;
    if (strcmp(s, "print") == 0) return TOK_PRINT;
    if (strcmp(s, "int") == 0) return TOK_INT;
    return -1;
}

TokenList lex(const char *src) {
    TokenList tl = { NULL, 0 };
    int cap = 64;
    tl.tokens = malloc(sizeof(Token) * cap);
    int i = 0, line = 1;

    while (src[i] != '\0') {
        char c = src[i];

        if (c == '\n') { line++; i++; continue; }
        if (isspace((unsigned char)c)) { i++; continue; }

        /* single line comments: // ... */
        if (c == '/' && src[i+1] == '/') {
            while (src[i] != '\0' && src[i] != '\n') i++;
            continue;
        }

        if (isdigit((unsigned char)c)) {
            int start = i;
            while (isdigit((unsigned char)src[i])) i++;
            Token t = {0};
            t.type = TOK_NUM;
            int len = i - start;
            strncpy(t.text, src + start, len);
            t.text[len] = '\0';
            t.ival = atoi(t.text);
            t.line = line;
            push(&tl, &cap, t);
            continue;
        }

        if (isalpha((unsigned char)c) || c == '_') {
            int start = i;
            while (isalnum((unsigned char)src[i]) || src[i] == '_') i++;
            Token t = {0};
            int len = i - start;
            strncpy(t.text, src + start, len);
            t.text[len] = '\0';
            int kw = keywordType(t.text);
            t.type = (kw != -1) ? kw : TOK_IDENT;
            t.line = line;
            push(&tl, &cap, t);
            continue;
        }

        Token t = {0};
        t.line = line;
        if (c == '+') { t.type = TOK_PLUS; strcpy(t.text, "+"); i++; }
        else if (c == '-') { t.type = TOK_MINUS; strcpy(t.text, "-"); i++; }
        else if (c == '*') { t.type = TOK_STAR; strcpy(t.text, "*"); i++; }
        else if (c == '/') { t.type = TOK_SLASH; strcpy(t.text, "/"); i++; }
        else if (c == '%') { t.type = TOK_PERCENT; strcpy(t.text, "%"); i++; }
        else if (c == ';') { t.type = TOK_SEMI; strcpy(t.text, ";"); i++; }
        else if (c == '(') { t.type = TOK_LPAREN; strcpy(t.text, "("); i++; }
        else if (c == ')') { t.type = TOK_RPAREN; strcpy(t.text, ")"); i++; }
        else if (c == '{') { t.type = TOK_LBRACE; strcpy(t.text, "{"); i++; }
        else if (c == '}') { t.type = TOK_RBRACE; strcpy(t.text, "}"); i++; }
        else if (c == '=' && src[i+1] == '=') { t.type = TOK_EQ; strcpy(t.text, "=="); i += 2; }
        else if (c == '=') { t.type = TOK_ASSIGN; strcpy(t.text, "="); i++; }
        else if (c == '!' && src[i+1] == '=') { t.type = TOK_NE; strcpy(t.text, "!="); i += 2; }
        else if (c == '!') { t.type = TOK_NOT; strcpy(t.text, "!"); i++; }
        else if (c == '<' && src[i+1] == '=') { t.type = TOK_LE; strcpy(t.text, "<="); i += 2; }
        else if (c == '<') { t.type = TOK_LT; strcpy(t.text, "<"); i++; }
        else if (c == '>' && src[i+1] == '=') { t.type = TOK_GE; strcpy(t.text, ">="); i += 2; }
        else if (c == '>') { t.type = TOK_GT; strcpy(t.text, ">"); i++; }
        else if (c == '&' && src[i+1] == '&') { t.type = TOK_AND; strcpy(t.text, "&&"); i += 2; }
        else if (c == '|' && src[i+1] == '|') { t.type = TOK_OR; strcpy(t.text, "||"); i += 2; }
        else {
            fprintf(stderr, "Lexer error: unexpected character '%c' at line %d\n", c, line);
            exit(1);
        }
        push(&tl, &cap, t);
    }

    Token eof = {0};
    eof.type = TOK_EOF;
    strcpy(eof.text, "EOF");
    eof.line = line;
    push(&tl, &cap, eof);

    return tl;
}

void freeTokenList(TokenList *tl) {
    free(tl->tokens);
    tl->tokens = NULL;
    tl->count = 0;
}

static const char* typeName(TokenType t) {
    switch (t) {
        case TOK_NUM: return "NUMBER";
        case TOK_IDENT: return "IDENT";
        case TOK_PLUS: return "PLUS";
        case TOK_MINUS: return "MINUS";
        case TOK_STAR: return "STAR";
        case TOK_SLASH: return "SLASH";
        case TOK_PERCENT: return "PERCENT";
        case TOK_ASSIGN: return "ASSIGN";
        case TOK_SEMI: return "SEMI";
        case TOK_LPAREN: return "LPAREN";
        case TOK_RPAREN: return "RPAREN";
        case TOK_LBRACE: return "LBRACE";
        case TOK_RBRACE: return "RBRACE";
        case TOK_LT: return "LT";
        case TOK_GT: return "GT";
        case TOK_LE: return "LE";
        case TOK_GE: return "GE";
        case TOK_EQ: return "EQ";
        case TOK_NE: return "NE";
        case TOK_AND: return "AND";
        case TOK_OR: return "OR";
        case TOK_NOT: return "NOT";
        case TOK_IF: return "IF";
        case TOK_ELSE: return "ELSE";
        case TOK_WHILE: return "WHILE";
        case TOK_FOR: return "FOR";
        case TOK_PRINT: return "PRINT";
        case TOK_INT: return "INT";
        case TOK_EOF: return "EOF";
    }
    return "?";
}

void printTokens(TokenList *tl) {
    for (int i = 0; i < tl->count; i++) {
        Token t = tl->tokens[i];
        printf("  [line %2d] %-8s '%s'\n", t.line, typeName(t.type), t.text);
    }
}
