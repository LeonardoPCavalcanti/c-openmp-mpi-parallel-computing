#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// --- Parâmetros da Simulação ---
const int PONTOS_TOTAIS = 1000000;
const int PASSOS_DE_TEMPO = 5000;
const double CONSTANTE_DE_DIFUSAO = 0.1;
const double TEMPERATURA_INICIAL_CENTRO = 100.0;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int num_processos, rank_processo;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_processo);

    const int contagem_pontos_locais = PONTOS_TOTAIS / num_processos;

    // Aloca memória local + 2 "pontos fantasma" para as fronteiras com vizinhos.
    double* temperatura = (double*)malloc((contagem_pontos_locais + 2) * sizeof(double));
    double* nova_temperatura = (double*)malloc((contagem_pontos_locais + 2) * sizeof(double));

    // --- Condições Iniciais ---
    for (int i = 1; i <= contagem_pontos_locais; i++) {
        int indice_global = rank_processo * contagem_pontos_locais + i - 1;
        if (indice_global > PONTOS_TOTAIS / 2 - PONTOS_TOTAIS / 10 && indice_global < PONTOS_TOTAIS / 2 + PONTOS_TOTAIS / 10) {
            temperatura[i] = TEMPERATURA_INICIAL_CENTRO;
        } else {
            temperatura[i] = 0.0;
        }
    }

    double tempo_inicio = MPI_Wtime();

    for (int passo = 0; passo < PASSOS_DE_TEMPO; passo++) {
        
        // --- Fase de Comunicação (Bloqueante) ---
        // Troca os pontos de fronteira com os processos vizinhos.
        // O programa espera a conclusão de cada envio/recebimento antes de continuar.
        if (rank_processo < num_processos - 1) {
            MPI_Send(&temperatura[contagem_pontos_locais], 1, MPI_DOUBLE, rank_processo + 1, 0, MPI_COMM_WORLD);
        }
        if (rank_processo > 0) {
            MPI_Recv(&temperatura[0], 1, MPI_DOUBLE, rank_processo - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank_processo > 0) {
            MPI_Send(&temperatura[1], 1, MPI_DOUBLE, rank_processo - 1, 1, MPI_COMM_WORLD);
        }
        if (rank_processo < num_processos - 1) {
            MPI_Recv(&temperatura[contagem_pontos_locais + 1], 1, MPI_DOUBLE, rank_processo + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        
        // --- Fase de Computação ---
        for (int i = 1; i <= contagem_pontos_locais; i++) {
            if ((rank_processo == 0 && i == 1) || (rank_processo == num_processos - 1 && i == contagem_pontos_locais)) continue;
            nova_temperatura[i] = temperatura[i] + CONSTANTE_DE_DIFUSAO * (temperatura[i - 1] - 2.0 * temperatura[i] + temperatura[i + 1]);
        }

        double* ponteiro_temp = temperatura;
        temperatura = nova_temperatura;
        nova_temperatura = ponteiro_temp;
    }

    double tempo_fim = MPI_Wtime();

    if (rank_processo == 0) {
        printf("Versão Bloqueante: Tempo de execução = %f segundos\n", tempo_fim - tempo_inicio);
    }

    free(temperatura);
    free(nova_temperatura);
    MPI_Finalize();
    return 0;
}