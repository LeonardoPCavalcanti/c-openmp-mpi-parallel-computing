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
        MPI_Request requisicoes[4];
        int contagem_requisicoes = 0;

        // 1. Inicia a comunicação não-bloqueante (não espera a conclusão).
        if (rank_processo > 0) {
            MPI_Irecv(&temperatura[0], 1, MPI_DOUBLE, rank_processo - 1, 0, MPI_COMM_WORLD, &requisicoes[contagem_requisicoes++]);
            MPI_Isend(&temperatura[1], 1, MPI_DOUBLE, rank_processo - 1, 1, MPI_COMM_WORLD, &requisicoes[contagem_requisicoes++]);
        }
        if (rank_processo < num_processos - 1) {
            MPI_Irecv(&temperatura[contagem_pontos_locais + 1], 1, MPI_DOUBLE, rank_processo + 1, 1, MPI_COMM_WORLD, &requisicoes[contagem_requisicoes++]);
            MPI_Isend(&temperatura[contagem_pontos_locais], 1, MPI_DOUBLE, rank_processo + 1, 0, MPI_COMM_WORLD, &requisicoes[contagem_requisicoes++]);
        }

        // 2. Sobreposição: Calcula os pontos internos enquanto a comunicação está em andamento.
        for (int i = 2; i < contagem_pontos_locais; i++) {
            nova_temperatura[i] = temperatura[i] + CONSTANTE_DE_DIFUSAO * (temperatura[i - 1] - 2.0 * temperatura[i] + temperatura[i + 1]);
        }

        // 3. Sincroniza: Espera que todas as comunicações pendentes terminem.
        MPI_Waitall(contagem_requisicoes, requisicoes, MPI_STATUSES_IGNORE);

        // 4. Calcula os pontos de fronteira, que dependiam dos dados recém-chegados.
        if (contagem_pontos_locais > 0) {
            if (rank_processo > 0) {
                nova_temperatura[1] = temperatura[1] + CONSTANTE_DE_DIFUSAO * (temperatura[0] - 2.0 * temperatura[1] + temperatura[2]);
            }
            if (rank_processo < num_processos - 1) {
                nova_temperatura[contagem_pontos_locais] = temperatura[contagem_pontos_locais] + CONSTANTE_DE_DIFUSAO * (temperatura[contagem_pontos_locais - 1] - 2.0 * temperatura[contagem_pontos_locais] + temperatura[contagem_pontos_locais + 1]);
            }
        }
        
        double* ponteiro_temp = temperatura;
        temperatura = nova_temperatura;
        nova_temperatura = ponteiro_temp;
    }

    double tempo_fim = MPI_Wtime();

    if (rank_processo == 0) {
        printf("Versão Não-Bloqueante (Wait): Tempo de execução = %f segundos\n", tempo_fim - tempo_inicio);
    }

    free(temperatura);
    free(nova_temperatura);
    MPI_Finalize();
    return 0;
}