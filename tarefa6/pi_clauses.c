#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define TOTAL_PONTOS 1000000

int main(void) {
    long long dentro_circulo_total = 0;
    unsigned int semente_base = (unsigned int)time(NULL);

    double inicio = omp_get_wtime();

    #pragma omp parallel default(none) shared(dentro_circulo_total, semente_base)
    {
        unsigned int semente_thread = semente_base ^ (unsigned int)omp_get_thread_num();
        long long dentro_circulo_local = 0;

        #pragma omp for
        for (long long i = 0; i < TOTAL_PONTOS; i++) {
            double x = (double)rand_r(&semente_thread) / RAND_MAX;
            double y = (double)rand_r(&semente_thread) / RAND_MAX;
            if (x*x + y*y < 1.0) dentro_circulo_local++;
        }

        #pragma omp critical
        dentro_circulo_total += dentro_circulo_local;
    }

    double fim = omp_get_wtime();
    double pi = 4.0 * (double)dentro_circulo_total / (double)TOTAL_PONTOS;
    printf("[Clauses] N=%d | pi=%.6f | tempo=%.6f s\n",
           TOTAL_PONTOS, pi, fim - inicio);
    return 0;
}
