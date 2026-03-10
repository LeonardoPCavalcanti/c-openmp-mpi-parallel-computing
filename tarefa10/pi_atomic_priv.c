#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

static const long DEFAULT_N = 100000000L;

int main(int argc, char **argv) {
    long N = (argc > 1) ? atoll(argv[1]) : DEFAULT_N;
    long global = 0;

    printf("[V4] Privado + atomic (somatório final) | N = %ld\n", N);
    double t0 = omp_get_wtime();

    #pragma omp parallel
    {
        long local = 0;
        unsigned int seed = (unsigned int)(time(NULL) ^ (0x9e3779b9u * (omp_get_thread_num()+1)));
        #pragma omp for
        for (long i = 0; i < N; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            if (x*x + y*y < 1.0) local++;
        }
        #pragma omp atomic
        global += local;
    }

    double t1 = omp_get_wtime();
    double pi = 4.0 * (double)global / (double)N;
    printf("Pi = %.8f | tempo = %.6f s\n", pi, t1 - t0);
    return 0;
}
