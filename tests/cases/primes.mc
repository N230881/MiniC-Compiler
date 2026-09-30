// primes below 30 using trial division
for (int n = 2; n < 30; n = n + 1) {
    int isPrime = 1;
    for (int d = 2; d * d <= n; d = d + 1) {
        if (n % d == 0) {
            isPrime = 0;
        }
    }
    if (isPrime) {
        print(n);
    }
}
