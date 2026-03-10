#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

static const long DEFAULT_N = 100000000L;

int main(int argc, char **argv) {
    long N = (argc > 1) ? atoll(argv[1]) : DEFAULT_N;
    long pontos = 0;

    printf("[V5] reduction (+:pontos) | N = %ld\n", N);
    double t0 = omp_get_wtime();

    #pragma omp parallel
    {
        unsigned int seed = (unsigned int)(time(NULL) ^ (0x9e3779b9u * (omp_get_thread_num()+1)));
        #pragma omp for reduction(+:pontos)
        for (long i = 0; i < N; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            if (x*x + y*y < 1.0) pontos++;
        }
    }

    double t1 = omp_get_wtime();
    double pi = 4.0 * (double)pontos / (double)N;
    printf("Pi = %.8f | tempo = %.6f s\n", pi, t1 - t0);
    return 0;
}
