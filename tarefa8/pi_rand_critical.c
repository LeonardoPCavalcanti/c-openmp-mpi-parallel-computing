// pi_rand_critical.c
// gcc -O2 -fopenmp pi_rand_critical.c -o pi_rand_critical
// ./pi_rand_critical [N]

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

static inline double rnd01_rand(void) {
    return (double)rand() / (double)RAND_MAX;
}

int main(int argc, char **argv) {
    const long long N = (argc > 1) ? atoll(argv[1]) : 10000000LL;

    long long total_in = 0;
    double t0 = omp_get_wtime();

    #pragma omp parallel
    {
        const int T   = omp_get_num_threads();
        const int tid = omp_get_thread_num();
        long long in_local = 0;

        for (long long i = tid; i < N; i += T) {
            double x = rnd01_rand();
            double y = rnd01_rand();
            if (x*x + y*y <= 1.0) in_local++;
        }

        #pragma omp critical
        total_in += in_local;
    }

    double pi = 4.0 * (double)total_in / (double)N;
    double t1 = omp_get_wtime();

    printf("[rand  + critical] N=%lld | pi=%.6f | tempo=%.6f s\n",
        N, pi, (t1 - t0));

    return 0;
}
