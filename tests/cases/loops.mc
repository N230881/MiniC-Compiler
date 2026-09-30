// factorial with while, nested for loops, countdown
int f = 1;
int i = 1;
while (i <= 5) {
    f = f * i;
    i = i + 1;
}
print(f);

int total = 0;
for (int r = 1; r <= 3; r = r + 1) {
    for (int c = 1; c <= 3; c = c + 1) {
        total = total + r * c;
    }
}
print(total);

int k = 3;
while (k > 0) {
    print(k);
    k = k - 1;
}
