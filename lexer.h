#ifndef LEXER_H
#define LEXER_H
#include "token.h"

typedef struct {
    Token *tokens;
    int count;
} TokenList;

TokenList lex(const char *source);
void freeTokenList(TokenList *tl);
void printTokens(TokenList *tl);

#endif
