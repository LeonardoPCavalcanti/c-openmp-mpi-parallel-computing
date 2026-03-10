#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define LIMITE_PADRAO 300000u 

static double tempo_agora(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

static int eh_primo(unsigned int num) {
    if (num < 2) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;
    unsigned int raiz = (unsigned int)floor(sqrt((double)num));
    for (unsigned int d = 3; d <= raiz; d += 2)
        if (num % d == 0) return 0;
    return 1;
}

int main(void) {
    unsigned int limite = LIMITE_PADRAO;  

    printf("Contando primos em [2, %u] (sequencial)\n\n", limite);

    unsigned long qtd_primos = 0;
    double t0 = tempo_agora();
    for (unsigned int k = 2; k <= limite; ++k)
        qtd_primos += eh_primo(k);
    double t1 = tempo_agora();

    printf("[SEQ] primos = %lu | tempo = %.6f s\n", qtd_primos, t1 - t0);
    return 0;
}
