#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_REPETICOES 100

int main(int argc, char *argv[]) {
    int meu_id, total_de_processos;
    double tempo_inicio, tempo_fim;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_id);
    MPI_Comm_size(MPI_COMM_WORLD, &total_de_processos);

    if (total_de_processos != 2) {
        if (meu_id == 0) fprintf(stderr, "Erro: Este programa deve rodar com exatamente 2 processos.\n");
        MPI_Finalize();
        return 1;
    }
    if (argc != 2) {
        if (meu_id == 0) fprintf(stderr, "Uso: mpirun -np 2 %s <tamanho_em_bytes>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int tamanho_da_mensagem = atoi(argv[1]);
    if (tamanho_da_mensagem == 0) tamanho_da_mensagem = 1;
    
    char *buffer = (char*)malloc(tamanho_da_mensagem);

    if (meu_id == 0) {
        tempo_inicio = MPI_Wtime();
        for (int i = 0; i < NUM_REPETICOES; i++) {
            MPI_Send(buffer, tamanho_da_mensagem, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buffer, tamanho_da_mensagem, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        tempo_fim = MPI_Wtime();

        double tempo_total = tempo_fim - tempo_inicio;
        double tempo_medio_ida_e_volta = tempo_total / NUM_REPETICOES;
        double tempo_uma_viagem = tempo_medio_ida_e_volta / 2.0;

        printf("%d,%.9f\n", tamanho_da_mensagem, tempo_uma_viagem * 1000);
    } else { // meu_id == 1
        for (int i = 0; i < NUM_REPETICOES; i++) {
            MPI_Recv(buffer, tamanho_da_mensagem, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buffer, tamanho_da_mensagem, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }

    free(buffer);
    MPI_Finalize();
    return 0;
}