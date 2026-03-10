#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Preenche matriz A e vetor x com valores iniciais
static void inicializar(double *A, double *x, int N) {
    for (int i = 0; i < N; i++) {
        x[i] = 1.0;
        for (int j = 0; j < N; j++) A[i*(long)N + j] = (i + j) * 0.001;
    }
}

// Multiplicação MxV percorrendo a matriz por colunas (cache-unfriendly)
static void mxv_por_colunas(const double *A, const double *x, double *y, int N) {
    memset(y, 0, N*sizeof(double));
    for (int j = 0; j < N; j++) {
        double xj = x[j];   // pega elemento da coluna j
        for (int i = 0; i < N; i++) 
            y[i] += A[i*(long)N + j] * xj;
    }
}

// Calcula um checksum simples para validar o resultado
static double checksum(const double *v, int N) {
    double s = 0.0; 
    for (int i = 0; i < N; i++) s += v[i]*(i+1); 
    return s;
}

int main(int argc, char **argv) {
    int N = (argc > 1) ? atoi(argv[1]) : 1024;
    size_t bytesA = (size_t)N*(size_t)N*sizeof(double);

    double *A = (double*)malloc(bytesA);
    double *x = (double*)malloc(N*sizeof(double));
    double *y = (double*)malloc(N*sizeof(double));
    if (!A || !x || !y) { fprintf(stderr,"Memória insuficiente para N=%d\n", N); return 1; }

    inicializar(A, x, N);

    double t0 = omp_get_wtime();
    mxv_por_colunas(A, x, y, N);
    double t1 = omp_get_wtime();

    printf("MxV por COLUNAS | N=%d | tempo=%.3f ms | checksum=%.2f\n",
           N, (t1 - t0)*1000.0, checksum(y, N));

    free(A); free(x); free(y);
    return 0;
}
