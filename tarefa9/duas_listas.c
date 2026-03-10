#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

typedef struct No {
    int valor;
    struct No* prox;
} No;

typedef struct {
    No* cabeca;
} Lista;

static void inserir_inicio(Lista* L, int v) {
    No* n = (No*)malloc(sizeof(No));
    if (!n) {
        fprintf(stderr, "Falha de memória\n");
        return;
    }
    n->valor  = v;
    n->prox   = L->cabeca;
    L->cabeca = n;
}

static long contar(const Lista* L) {
    long c = 0;
    for (const No* p = L->cabeca; p; p = p->prox) {
        c++;
    }
    return c;
}

static void liberar(Lista* L) {
    No* p = L->cabeca;
    while (p) {
        No* nx = p->prox;
        free(p);
        p = nx;
    }
    L->cabeca = NULL;
}

int main(int argc, char** argv) {
    const int N = (argc > 1) ? atoi(argv[1]) : 100000;

    Lista A = { .cabeca = NULL };
    Lista B = { .cabeca = NULL };

    #pragma omp parallel for
    for (int k = 0; k < N; k++) {
        unsigned int seed = (unsigned int)time(NULL) ^ (unsigned)omp_get_thread_num() ^ (unsigned)k;

        int valor = (int)(rand_r(&seed) % 1001);
        int qual  = (int)(rand_r(&seed) & 1);   // 0 -> A, 1 -> B

        if (qual == 0) {
            #pragma omp critical(lista_A)
            inserir_inicio(&A, valor);
        } else {
            #pragma omp critical(lista_B)
            inserir_inicio(&B, valor);
        }
    }

    long cA = contar(&A);
    long cB = contar(&B);

    printf("A = %ld, B = %ld, total = %ld (esperado = %d)\n",
           cA, cB, cA + cB, N);

    liberar(&A);
    liberar(&B);
    return 0;
}
