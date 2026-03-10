#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define TOTAL_PONTOS 1000000

int main(void) {
    long long dentro_circulo = 0;
    unsigned int semente_base = (unsigned int)time(NULL);

    double inicio = omp_get_wtime();

    #pragma omp parallel
    {
        unsigned int semente_thread = semente_base ^ (unsigned int)omp_get_thread_num();

        #pragma omp for
        for (long long i = 0; i < TOTAL_PONTOS; i++) {
            double x = (double)rand_r(&semente_thread) / RAND_MAX;
            double y = (double)rand_r(&semente_thread) / RAND_MAX;
            if (x*x + y*y < 1.0) {
                #pragma omp critical
                dentro_circulo++;
            }
        }
    }

    double fim = omp_get_wtime();
    double pi = 4.0 * (double)dentro_circulo / (double)TOTAL_PONTOS;
    printf("[Critical] N=%d | pi=%.6f | tempo=%.6f s\n",
           TOTAL_PONTOS, pi, fim - inicio);
    return 0;
}
