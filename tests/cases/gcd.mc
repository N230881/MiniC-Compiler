// Euclid's algorithm
int x = 252;
int y = 105;
while (y != 0) {
    int r = x % y;
    x = y;
    y = r;
}
print(x);
