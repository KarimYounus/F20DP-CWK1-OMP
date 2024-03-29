//
// Created by Karim Younus
// F20DP CWK1
// C & OpenMP Sum Totients Implementation
//

#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * Inspired by the TotientRange sequential implementation from the F20DP Gitlab
 * [source]: https://gitlab-student.macs.hw.ac.uk/f20dp/f20dp-totient-range/-/blob/master/TotientRange.c
*/

bool SEQ, BIN_GCD = false;  // Global bools for sequential execution and binary gcd calculation

long gcdEuclid(unsigned long a, unsigned long b) {
    /*
    - Function to calculate the greatest common divisor (GCD) of two numbers.
    The Euclidean algorithm as implemented in the sequential version of the algorithm provided in the F20DP Gitlab:
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
    // Function to determine if 2 numbers are relatively prime, using either the Euclidean or the binary algorithm to
    // find the GCD
    if (BIN_GCD) return gcdBinary(a, b) == 1;
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

long sumTotientsParallel(unsigned long lower, unsigned long upper) {
    // Function to sum the totients across a range of numbers from n=lower to n=upper

    unsigned sum = 0;
    unsigned n;

    // This represents the outermost loop of the program, thus we employ parallelization here.
    #pragma omp parallel private(n) shared(sum)
    {
        // Print the number of threads
        if (omp_get_thread_num() == 0) {
            printf("Parallel Threads = %d\n", omp_get_num_threads());
        }
        // We use the sum clause to ensure that the sum variable is handled correctly by OpenMP to avoid any race conditions
        #pragma omp for reduction(+:sum)
        for (n = lower; n <= upper; n++)
            sum = sum + euler(n);
    }

    return sum;
}

long sumTotientsSequential(unsigned long lower, unsigned long upper) {
    // Function to sum the totients across a range of numbers from n=lower to n=upper

    unsigned sum = 0;
    unsigned n;

    // Print the number of threads
    if (omp_get_thread_num() == 0) {
        printf("Parallel Threads = %d\n", omp_get_num_threads());
    }
    for (n = lower; n <= upper; n++)
        sum = sum + euler(n);

    return sum;
}

omp_sched_t determine_sched(char* sched_type) {
    // Function to set the OpenMP scheduling strategy from the programs arguments

    if (strcmp(sched_type, "static") == 0) {
        return omp_sched_static;
    } else if (strcmp(sched_type, "dynamic") == 0) {
        return omp_sched_dynamic;
    } else if (strcmp(sched_type, "guided") == 0) {
        return omp_sched_guided;
    } else if (strcmp(sched_type, "auto") == 0) {
        return omp_sched_auto;
    } else {
        printf("Error: Invalid scheduling strategy - '%s'\n", sched_type);
        return omp_sched_static;
    }
}

void assert_sched() {
    // Function to check the current scheduling strategy being employed by OpenMP

    omp_sched_t current_sched;
    int current_chunk_s;

    omp_get_schedule(&current_sched, &current_chunk_s);

    // Convert the current_sched to a string
    const char* sched_str = "unknown";
    switch (current_sched) {
        case omp_sched_static:
            sched_str = "static";
            break;
        case omp_sched_dynamic:
            sched_str = "dynamic";
            break;
        case omp_sched_guided:
            sched_str = "guided";
            break;
        case omp_sched_auto:
            sched_str = "auto";
            break;
        case omp_sched_monotonic:
            sched_str = "monotonic";
            break;
    }
    printf("Scheduling strategy = %s, Chunk size = %d\n", sched_str, current_chunk_s);
}

void output_metrics(const char* filename, long lower, long upper, double exec_t, int n_cores, const char* gcd, const char* sched_t, int chunk_s) {
    FILE *fp;

    // Open the file. If the file does not exist, it will be created.
    fp = fopen(filename, "a");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    // Check if file is empty to write header
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size == 0) {
        fprintf(fp, "\"Filename\", \"Lower\', \"Upper\",\"Execution Time\", \"Core Count\", \"GCD Version\", \"Scheduling Strategy\", \"Chunk Size\"\n");
    }

    // Move back to the end of the file to append data
    fseek(fp, 0, SEEK_END);

    // Write the metrics to the file in CSV format
    fprintf(fp, "\"%s\", %ld, %ld, %f, %d, \"%s\", \"%s\", %d\n", filename, lower, upper, exec_t, n_cores, gcd, sched_t, chunk_s);

    // Close the file
    fclose(fp);
}

/*
 * ==================================================================================
 * MAIN
 * ==================================================================================
 */

int main(int argc, char ** argv) {
    unsigned long lower, upper; // Bounds of function
    int num_t, chunk_s = 1; // Thread count and scheduling chunk size - Default to 1
    omp_sched_t sched = omp_sched_static; // Scheduling strategy - Default to static
    unsigned long result; // Result of sum
    char *filename = NULL; // Filename - Default to null
    char *gcd_str = "Euclid"; // GCD Function String - Default to Euclid
    char *sched_str = "static"; // Scheduling startegy string - Default to static


    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seq") == 0) {    // Sequential?
            SEQ = true;
            continue;
        }
        if (strcmp(argv[i], "--binary") == 0) {  // Binary GCD?
            BIN_GCD = true;
            continue;
        }
        if (strcmp(argv[i], "--filename") == 0 && i + 1 < argc) {   // Filename? - check to ensure there is at least one more arg after the flag
            filename = argv[++i]; // Use the next arg as the filename
            continue;
        }
        if (i == 1) {    // Lower bound
            lower = strtoul(argv[i], NULL, 10);
        } else if (i == 2) {    // Upper bound
            upper = strtoul(argv[i], NULL, 10);
        } else if (i == 3) {    // Num of threads
            num_t = atoi(argv[i]);
        } else if (i == 4) {    // Scheduling strategy
            sched = determine_sched(argv[i]);
            sched_str = argv[i];
        } else if (i == 5) {    // Chunk size
            chunk_s = atoi(argv[i]);
        }
    }

    // If --seq flag is used, run the sequential version of the algorithm
    if (SEQ) {
        printf("\nRunning Sequentially . . .\n");
        result = sumTotientsSequential(lower, upper);
    }
    else printf("\nRunning in Parallel . . .\n");

    if (BIN_GCD) {
        printf("Using Binary GCD Calculation\n");
        gcd_str = "Binary";
    }
    else printf("Using Euclidean GCD Calculation\n");

    // Set number of threads and scheduling strategy as defined in the arguments
    omp_set_num_threads(num_t);
    omp_set_schedule(sched, chunk_s);

    // Check Scheduling
    assert_sched();

    double start_time = omp_get_wtime(); // Start timing
    result = sumTotientsParallel(lower, upper); // Run calculation
    double end_time = omp_get_wtime(); //Stop timing
    double exec_t = end_time - start_time;

    // Parse Metrics
    printf("\nRESULTS =======================\n");
    printf("Execution Time = %f seconds\n", exec_t);
    printf("Lower=%ld, Upper=%ld, Result=%ld\n", lower, upper, result);

    //TODO: Mind the compiler dependent type conversion below for lower and upper
    output_metrics(filename, lower, upper, exec_t, num_t, gcd_str, sched_str, chunk_s);
    return 0;
}
