// pi_rand_r_vector.c
// gcc -O2 -fopenmp pi_rand_r_vector.c -o pi_rand_r_vector
// ./pi_rand_r_vector [N]

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

static inline double rnd01_rand_r(unsigned int *seed) {
    return (double)rand_r(seed) / (double)RAND_MAX;
}

int main(int argc, char **argv) {
    const long long N = (argc > 1) ? atoll(argv[1]) : 10000000LL;

    const int maxT = omp_get_max_threads();
    long long *hits = (long long*)calloc((size_t)maxT, sizeof(long long));
    if (!hits) { fprintf(stderr, "Falha ao alocar hits.\n"); return 1; }

    double t0 = omp_get_wtime();

    #pragma omp parallel
    {
        const int T   = omp_get_num_threads();
        const int tid = omp_get_thread_num();

        unsigned int seed = (unsigned)time(NULL) + (unsigned)tid;


        long long in_local = 0;
        for (long long i = tid; i < N; i += T) {
            double x = rnd01_rand_r(&seed);
            double y = rnd01_rand_r(&seed);
            if (x*x + y*y <= 1.0) in_local++;
        }
        hits[tid] = in_local;   // sem critical
    }

    long long total_in = 0;
    for (int t = 0; t < maxT; t++) total_in += hits[t];

    double pi = 4.0 * (double)total_in / (double)N;
    double t1 = omp_get_wtime();

    printf("[rand_r + vetor   ] N=%lld | pi=%.6f | tempo=%.6f s\n",
         N, pi, (t1 - t0));

    free(hits);
    return 0;
}
