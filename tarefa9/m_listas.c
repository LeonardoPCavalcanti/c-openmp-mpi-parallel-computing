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
    const int M = (argc > 1) ? atoi(argv[1]) : 4;
    const int N = (argc > 2) ? atoi(argv[2]) : 100000;

    if (M <= 0) {
        fprintf(stderr, "Número de listas deve ser positivo.\n");
        return 1;
    }

    Lista* listas = (Lista*)calloc((size_t)M, sizeof(Lista));
    omp_lock_t* locks = (omp_lock_t*)malloc((size_t)M * sizeof(omp_lock_t));

    if (!listas || !locks) {
        fprintf(stderr, "Falha ao alocar listas/locks.\n");
        free(listas);
        free(locks);
        return 1;
    }

    for (int i = 0; i < M; i++) {
        listas[i].cabeca = NULL;
        omp_init_lock(&locks[i]);
    }

    #pragma omp parallel for
    for (int k = 0; k < N; k++) {
        unsigned int seed = (unsigned int)time(NULL) ^ (unsigned)omp_get_thread_num() ^ (unsigned)k;

        int valor = (int)(rand_r(&seed) % 1001);
        int idx   = (int)(rand_r(&seed) % (unsigned)M);

        omp_set_lock(&locks[idx]);
        inserir_inicio(&listas[idx], valor);
        omp_unset_lock(&locks[idx]);
    }

    long long total = 0;
    for (int i = 0; i < M; i++) {
        long qtd = contar(&listas[i]);
        printf("lista[%d] = %ld\n", i, qtd);
        total += qtd;
    }

    printf("Total = %lld (esperado = %d) em M = %d listas\n",
           total, N, M);

    for (int i = 0; i < M; i++) {
        omp_destroy_lock(&locks[i]);
        liberar(&listas[i]);
    }

    free(locks);
    free(listas);
    return 0;
}
