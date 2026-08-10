#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOK_NUM, TOK_IDENT,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_ASSIGN, TOK_SEMI, TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_LT, TOK_GT, TOK_LE, TOK_GE, TOK_EQ, TOK_NE,
    TOK_AND, TOK_OR, TOK_NOT,
    TOK_IF, TOK_ELSE, TOK_WHILE, TOK_FOR, TOK_PRINT, TOK_INT,
    TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char text[64];
    int ival;
    int line;
} Token;

#endif
