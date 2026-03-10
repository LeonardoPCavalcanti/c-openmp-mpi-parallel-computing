#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 100000000 

double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}

int main() {
    long long *vetor = (long long *) malloc(N * sizeof(long long));
    if (vetor == NULL) {
        printf("Erro ao alocar memória!\n");
        return 1;
    }

    double start_time = get_time();
    for (long long i = 0; i < N; i++) {
        vetor[i] = i * 2;
    }
    double end_time = get_time();
    printf("Tempo de inicializacao (Laço 1): %f segundos\n", end_time - start_time);

    long long soma_acumulativa = 0;
    start_time = get_time();
    for (long long i = 0; i < N; i++) {
        soma_acumulativa += vetor[i];  // dependência entre iterações
    }
    end_time = get_time();
    printf("Soma acumulativa (Laço 2): %lld\n", soma_acumulativa);
    printf("Tempo da soma acumulativa (Laço 2): %f segundos\n", end_time - start_time);

    long long soma1 = 0, soma2 = 0;
    start_time = get_time();
    for (long long i = 0; i < N; i += 2) {
        soma1 += vetor[i];
        if (i + 1 < N) {
            soma2 += vetor[i + 1];
        }
    }
    long long soma_total = soma1 + soma2;
    end_time = get_time();
    printf("Soma com paralelismo ILP (Laço 3): %lld\n", soma_total);
    printf("Tempo da soma com paralelismo ILP (Laço 3): %f segundos\n", end_time - start_time);

    if (soma_acumulativa != soma_total) {
        printf("Aviso: resultados diferentes!\n");
    }

    free(vetor);
    return 0;
}
