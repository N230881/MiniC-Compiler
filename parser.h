#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "ast.h"

Node* parseProgram(TokenList *tl);

#endif
