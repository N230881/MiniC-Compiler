#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static Token *toks;
static int pos;

static Token peek(void) { return toks[pos]; }
static Token advance(void) { return toks[pos++]; }

static void expect(TokenType t, const char *msg) {
    if (peek().type != t) {
        fprintf(stderr, "Parse error at line %d: expected %s, got '%s'\n",
                peek().line, msg, peek().text);
        exit(1);
    }
    advance();
}

static Node* parseExpr(void);
static Node* parseStatement(void);
static Node* parseBlock(void);

static Node* parsePrimary(void) {
    Token t = peek();
    if (t.type == TOK_NUM) {
        advance();
        Node *n = newNode(N_NUM);
        n->ival = t.ival;
        n->line = t.line;
        return n;
    }
    if (t.type == TOK_IDENT) {
        advance();
        Node *n = newNode(N_VAR);
        strcpy(n->name, t.text);
        n->line = t.line;
        return n;
    }
    if (t.type == TOK_LPAREN) {
        advance();
        Node *n = parseExpr();
        expect(TOK_RPAREN, ")");
        return n;
    }
    fprintf(stderr, "Parse error at line %d: unexpected token '%s'\n", t.line, t.text);
    exit(1);
}

static Node* parseUnary(void) {
    Token t = peek();
    if (t.type == TOK_MINUS || t.type == TOK_NOT) {
        advance();
        Node *n = newNode(N_UNOP);
        strcpy(n->op, t.text);
        n->a = parseUnary();
        n->line = t.line;
        return n;
    }
    return parsePrimary();
}

static Node* parseFactor(void) {
    Node *left = parseUnary();
    while (peek().type == TOK_STAR || peek().type == TOK_SLASH || peek().type == TOK_PERCENT) {
        Token op = advance();
        Node *n = newNode(N_BINOP);
        strcpy(n->op, op.text);
        n->a = left;
        n->b = parseUnary();
        n->line = op.line;
        left = n;
    }
    return left;
}

static Node* parseTerm(void) {
    Node *left = parseFactor();
    while (peek().type == TOK_PLUS || peek().type == TOK_MINUS) {
        Token op = advance();
        Node *n = newNode(N_BINOP);
        strcpy(n->op, op.text);
        n->a = left;
        n->b = parseFactor();
        n->line = op.line;
        left = n;
    }
    return left;
}

static Node* parseComparison(void) {
    Node *left = parseTerm();
    while (peek().type == TOK_LT || peek().type == TOK_GT || peek().type == TOK_LE || peek().type == TOK_GE) {
        Token op = advance();
        Node *n = newNode(N_BINOP);
        strcpy(n->op, op.text);
        n->a = left;
        n->b = parseTerm();
        n->line = op.line;
        left = n;
    }
    return left;
}

static Node* parseEquality(void) {
    Node *left = parseComparison();
    while (peek().type == TOK_EQ || peek().type == TOK_NE) {
        Token op = advance();
        Node *n = newNode(N_BINOP);
        strcpy(n->op, op.text);
        n->a = left;
        n->b = parseComparison();
        n->line = op.line;
        left = n;
    }
    return left;
}

static Node* parseLogicAnd(void) {
    Node *left = parseEquality();
    while (peek().type == TOK_AND) {
        Token op = advance();
        Node *n = newNode(N_BINOP);
        strcpy(n->op, "&&");
        n->a = left;
        n->b = parseEquality();
        n->line = op.line;
        left = n;
    }
    return left;
}

static Node* parseLogicOr(void) {
    Node *left = parseLogicAnd();
    while (peek().type == TOK_OR) {
        Token op = advance();
        Node *n = newNode(N_BINOP);
        strcpy(n->op, "||");
        n->a = left;
        n->b = parseLogicAnd();
        n->line = op.line;
        left = n;
    }
    return left;
}

static Node* parseExpr(void) {
    return parseLogicOr();
}

/* declaration or assignment used inside for(...) headers and normal statements */
static Node* parseDeclOrAssign(void) {
    if (peek().type == TOK_INT) {
        Token kw = advance();
        Token name = peek();
        expect(TOK_IDENT, "identifier");
        Node *n = newNode(N_DECL);
        strcpy(n->name, name.text);
        n->line = kw.line;
        if (peek().type == TOK_ASSIGN) {
            advance();
            n->a = parseExpr();
        }
        return n;
    } else {
        Token name = peek();
        expect(TOK_IDENT, "identifier");
        expect(TOK_ASSIGN, "=");
        Node *n = newNode(N_ASSIGN);
        strcpy(n->name, name.text);
        n->a = parseExpr();
        n->line = name.line;
        return n;
    }
}

static Node* parseStatement(void) {
    Token t = peek();

    if (t.type == TOK_LBRACE) return parseBlock();

    if (t.type == TOK_INT) {
        Node *n = parseDeclOrAssign();
        expect(TOK_SEMI, ";");
        return n;
    }

    if (t.type == TOK_IDENT) {
        Node *n = parseDeclOrAssign();
        expect(TOK_SEMI, ";");
        return n;
    }

    if (t.type == TOK_IF) {
        advance();
        expect(TOK_LPAREN, "(");
        Node *n = newNode(N_IF);
        n->line = t.line;
        n->a = parseExpr();
        expect(TOK_RPAREN, ")");
        n->b = parseStatement();
        if (peek().type == TOK_ELSE) {
            advance();
            n->c = parseStatement();
        }
        return n;
    }

    if (t.type == TOK_WHILE) {
        advance();
        expect(TOK_LPAREN, "(");
        Node *n = newNode(N_WHILE);
        n->line = t.line;
        n->a = parseExpr();
        expect(TOK_RPAREN, ")");
        n->b = parseStatement();
        return n;
    }

    if (t.type == TOK_FOR) {
        advance();
        expect(TOK_LPAREN, "(");
        Node *n = newNode(N_FOR);
        n->line = t.line;
        n->a = parseDeclOrAssign();
        expect(TOK_SEMI, ";");
        n->b = parseExpr();
        expect(TOK_SEMI, ";");
        n->c = parseDeclOrAssign();
        expect(TOK_RPAREN, ")");
        n->d = parseStatement();
        return n;
    }

    if (t.type == TOK_PRINT) {
        advance();
        expect(TOK_LPAREN, "(");
        Node *n = newNode(N_PRINT);
        n->line = t.line;
        n->a = parseExpr();
        expect(TOK_RPAREN, ")");
        expect(TOK_SEMI, ";");
        return n;
    }

    fprintf(stderr, "Parse error at line %d: unexpected token '%s'\n", t.line, t.text);
    exit(1);
}

static Node* parseBlock(void) {
    expect(TOK_LBRACE, "{");
    Node *block = newNode(N_BLOCK);
    while (peek().type != TOK_RBRACE && peek().type != TOK_EOF) {
        addStmt(block, parseStatement());
    }
    expect(TOK_RBRACE, "}");
    return block;
}

Node* parseProgram(TokenList *tl) {
    toks = tl->tokens;
    pos = 0;
    Node *prog = newNode(N_PROGRAM);
    while (peek().type != TOK_EOF) {
        addStmt(prog, parseStatement());
    }
    return prog;
}
