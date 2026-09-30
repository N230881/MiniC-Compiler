int a = 0;
int b = 1;
int count = 0;
while (count < 10) {
    print(a);
    int t = a + b;
    a = b;
    b = t;
    count = count + 1;
}
