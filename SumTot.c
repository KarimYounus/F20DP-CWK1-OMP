//
// Created by Karim Younus
// F20DP CWK1
// C & OpenMP Sum Totients Implementation

#include <stdio.h>
#include <omp.h>

long gcdEuclid(unsigned long a, unsigned long b) {
    /*
    - Function to calculate the greatest common divisor (GCD) of two numbers.
    The Euclidean algorithm as implemented in the serial version of the algorithm provided in the F20DP Gitlab:
    [source]: https://gitlab-student.macs.hw.ac.uk/f20dp/f20dp-totient-range/-/blob/master/TotientRange.c
     */
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

long gcdBinary(unsigned long a, unsigned long b) {
    /*
    - Function to calculate the greatest common divisor (GCD) of two numbers.
    The Binary GCD, or Stein's algorithm, making use of the built-in `__builtin_ctzl`function to get
    the number of trailing zeros. Implementation drawn from the work of Steven Pigeon and Daniel Lemire.
    [source - S Pigeon]: https://hbfs.wordpress.com/2013/12/10/the-speed-of-gcd/
    [source - D Lemire]: https://lemire.me/blog/2013/12/26/fastest-way-to-compute-the-greatest-common-divisor/
    */
    if (a == 0) return b;
    if (b == 0) return a;

    long shift = __builtin_ctzl(a|b);
    a >>= __builtin_ctzl(a);

    do
    {
        b >>= __builtin_ctzl(b);
        b -= a;
        unsigned long c = (long)b >> 63;
        a += b & c;
        b = (b + c) ^ c;
    } while (b != 0);

    return a << shift;
}

int relPrime(unsigned long a, unsigned long b) {
    // Function to determine if 2 numbers are relatively prime
    return gcdEuclid(a, b) == 1;
}

long euler(unsigned long n) {
    // Function to count the numbers up to n that are relatively prime to n

    unsigned long count = 0;
    unsigned long i;

    for (i = 1; i < n; i++)
        if (relPrime(i, n))
            count++;

    return count;
}

long sumTotients(unsigned long lower, unsigned long upper) {
    // Function to sum the totients across a range of numbers from n=lower to n=upper

    unsigned sum = 0;
    unsigned n;

    // This function represents the outermost loop of the program, thus we employ loop parallelization here.
    // We use the sum clause to ensure that the sum variable is handled correctly by OpenMP to avoid any race conditions
    #pragma omp parallel for reduction(+:sum)
    for (n = lower; n <= upper; n++)
        sum = sum + euler(n);
    return sum;
}

int main(int argc, char ** argv) {
    unsigned long lower, upper;

    if (argc != 3) {
        printf("Error: 2 Arguments Required");
        return 0;
    }
    sscanf(argv[1], "%ld", &lower);
    sscanf(argv[2], "%ld", &upper);
    unsigned long result = sumTotients(lower, upper);

    printf("Lower=%ld, Upper=%ld, Result=%ld\n", lower, upper, result);

    return 1;
}
