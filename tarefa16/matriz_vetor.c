#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void inicializar_dados(double *matriz, double *vetor, int linhas, int colunas);

int main(int argc, char *argv[]) {
    int meu_rank, num_processos;
    double tempo_inicio, tempo_fim;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processos);

    if (argc != 2) {
        if (meu_rank == 0) fprintf(stderr, "Uso: %s <tamanho_matriz_quadrada>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }
    
    int NUM_LINHAS = atoi(argv[1]);
    int NUM_COLUNAS = NUM_LINHAS;

    if (meu_rank == 0 && (NUM_LINHAS % num_processos != 0)) {
        fprintf(stderr, "ERRO: O número de linhas (%d) não é divisível pelo número de processos (%d).\n", NUM_LINHAS, num_processos);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    int linhas_por_processo = NUM_LINHAS / num_processos;
    double *matriz_global_A = NULL, *vetor_resultado_y = NULL;
    
    if (meu_rank == 0) {
        matriz_global_A = (double *)malloc(NUM_LINHAS * NUM_COLUNAS * sizeof(double));
        vetor_resultado_y = (double *)malloc(NUM_LINHAS * sizeof(double));
    }

    double *vetor_x = (double *)malloc(NUM_COLUNAS * sizeof(double));
    double *bloco_local_A = (double *)malloc(linhas_por_processo * NUM_COLUNAS * sizeof(double));
    double *resultado_local_y = (double *)malloc(linhas_por_processo * sizeof(double));

    if (meu_rank == 0) {
        inicializar_dados(matriz_global_A, vetor_x, NUM_LINHAS, NUM_COLUNAS);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (meu_rank == 0) tempo_inicio = MPI_Wtime();

    MPI_Bcast(vetor_x, NUM_COLUNAS, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(matriz_global_A, linhas_por_processo * NUM_COLUNAS, MPI_DOUBLE,
                bloco_local_A, linhas_por_processo * NUM_COLUNAS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < linhas_por_processo; i++) {
        resultado_local_y[i] = 0.0;
        for (int j = 0; j < NUM_COLUNAS; j++) {
            resultado_local_y[i] += bloco_local_A[i * NUM_COLUNAS + j] * vetor_x[j];
        }
    }

    MPI_Gather(resultado_local_y, linhas_por_processo, MPI_DOUBLE,
               vetor_resultado_y, linhas_por_processo, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (meu_rank == 0) {
        tempo_fim = MPI_Wtime();
        printf("TEMPO:%.6f\n", tempo_fim - tempo_inicio);
    }

    free(bloco_local_A);
    free(resultado_local_y);
    free(vetor_x);
    if (meu_rank == 0) {
        free(matriz_global_A);
        free(vetor_resultado_y);
    }

    MPI_Finalize();
    return 0;
}

void inicializar_dados(double *matriz, double *vetor, int linhas, int colunas) {
    srand(time(NULL));
    for (int i = 0; i < linhas * colunas; i++) matriz[i] = (double)(rand() % 10);
    for (int j = 0; j < colunas; j++) vetor[j] = (double)(rand() % 10);
}