#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

#define LIMITE_PADRAO 300000u   
#define MAX_THREADS   8         

static double agora(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

static int eh_primo(unsigned int x) {
    if (x < 2) return 0;
    if (x == 2) return 1;
    if (x % 2 == 0) return 0;
    unsigned int limite = (unsigned int) floor(sqrt((double)x));
    for (unsigned int d = 3; d <= limite; d += 2)
        if (x % d == 0) return 0;
    return 1;
}

int main(void) {
    unsigned int limite = LIMITE_PADRAO;

    printf("Contando primos em [2, %u] (paralelo naive)\n\n", limite);

    for (int num_threads = 1; num_threads <= MAX_THREADS; num_threads++) {
        omp_set_num_threads(num_threads);

        unsigned long contador = 0;   // variável compartilhada → sujeito a erro
        double inicio = agora();

        #pragma omp parallel for
        for (unsigned int k = 2; k <= limite; k++) {
            if (eh_primo(k)) contador++;  
        }

        double fim = agora();
        printf("[OMP] threads=%d | primos=%lu | tempo=%.6f s  (SUJEITO A ERRO)\n",
               num_threads, contador, fim - inicio);
    }

    return 0;
}
