#ifndef OPS_H
#define OPS_H

/* Applies a binary operator (as text, e.g. "+", "<=", "&&") to two ints. */
int applyBinOp(const char *op, int a, int b);

/* Applies a unary operator ("-" or "!") to one int. */
int applyUnOp(const char *op, int a);

#endif
