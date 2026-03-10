#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define TOTAL_PONTOS 1000000

int main(void) {
    long long dentro_circulo = 0;
    unsigned int semente = 12345u;  // semente fixa -> resultados reproduzíveis

    double inicio = omp_get_wtime();
    for (long long i = 0; i < TOTAL_PONTOS; i++) {
        double x = (double)rand_r(&semente) / RAND_MAX;
        double y = (double)rand_r(&semente) / RAND_MAX;
        if (x*x + y*y < 1.0) dentro_circulo++;
    }
    double fim = omp_get_wtime();

    double pi = 4.0 * (double)dentro_circulo / (double)TOTAL_PONTOS;
    printf("[Sequencial] N=%d | pi=%.6f | tempo=%.6f s\n",
           TOTAL_PONTOS, pi, fim - inicio);
    return 0;
}
