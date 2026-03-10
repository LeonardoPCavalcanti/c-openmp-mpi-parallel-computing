#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define SIDE        10000              // matriz SIDE x SIDE
#define MAX_THREADS 8
#define INDEX(i,j)  ((long long)(i) * SIDE + (j))  // row-major

static void shuffle_indices(int *v, int n) {
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}

int main(void) {
    double *matA = (double*) malloc((long long)SIDE * SIDE * sizeof(double));
    double *matB = (double*) malloc((long long)SIDE * SIDE * sizeof(double));
    double *matC = (double*) malloc((long long)SIDE * SIDE * sizeof(double));
    int    *col_order = (int*) malloc(SIDE * sizeof(int));

    if (matA == NULL || matB == NULL || matC == NULL || col_order == NULL) {
        fprintf(stderr, "Erro ao alocar memoria.\n");
        return 1;
    }

    // inicializa matrizes
    #pragma omp parallel for
    for (int i = 0; i < SIDE; ++i) {
        for (int j = 0; j < SIDE; ++j) {
            matA[INDEX(i,j)] = 1.0;
            matB[INDEX(i,j)] = 2.0;
        }
    }
    for (int j = 0; j < SIDE; ++j) col_order[j] = j;

    srand((unsigned) time(NULL));
    shuffle_indices(col_order, SIDE);

    printf("MEMORY-BOUND (matrix col-major) | %dx%d\n", SIDE, SIDE);

    for (int threads = 1; threads <= MAX_THREADS; ++threads) {
        omp_set_num_threads(threads);

        double t0 = omp_get_wtime();

        #pragma omp parallel for
        for (int k = 0; k < SIDE; ++k) {
            int col = col_order[k];
            for (int row = 0; row < SIDE; ++row) {
                matC[INDEX(row,col)] = matA[INDEX(row,col)] + matB[INDEX(row,col)];
            }
        }

        double t1 = omp_get_wtime();

        double checksum = 0.0;
        for (int row = 0; row < SIDE; ++row) checksum += matC[INDEX(row,0)];

        printf("threads=%d | tempo=%.6f s | check=%.1f\n",
               threads, t1 - t0, checksum);
    }

    free(matA);
    free(matB);
    free(matC);
    free(col_order);
    return 0;
}
