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
        MPI_Request requisicoes_recebimento[2], requisicoes_envio[2];
        int contagem_recebimento = 0, contagem_envio = 0;

        // 1. Inicia a comunicação não-bloqueante.
        if (rank_processo > 0) {
            MPI_Irecv(&temperatura[0], 1, MPI_DOUBLE, rank_processo - 1, 0, MPI_COMM_WORLD, &requisicoes_recebimento[contagem_recebimento++]);
            MPI_Isend(&temperatura[1], 1, MPI_DOUBLE, rank_processo - 1, 1, MPI_COMM_WORLD, &requisicoes_envio[contagem_envio++]);
        }
        if (rank_processo < num_processos - 1) {
            MPI_Irecv(&temperatura[contagem_pontos_locais + 1], 1, MPI_DOUBLE, rank_processo + 1, 1, MPI_COMM_WORLD, &requisicoes_recebimento[contagem_recebimento++]);
            MPI_Isend(&temperatura[contagem_pontos_locais], 1, MPI_DOUBLE, rank_processo + 1, 0, MPI_COMM_WORLD, &requisicoes_envio[contagem_envio++]);
        }

        // 2. Sobreposição: Calcula os pontos internos.
        for (int i = 2; i < contagem_pontos_locais; i++) {
             nova_temperatura[i] = temperatura[i] + CONSTANTE_DE_DIFUSAO * (temperatura[i - 1] - 2.0 * temperatura[i] + temperatura[i + 1]);
        }

        // 3. Sincroniza: Verifica ativamente se os recebimentos terminaram.
        int fantasma_esquerdo_recebido = (rank_processo == 0);
        int fantasma_direito_recebido = (rank_processo == num_processos - 1);
        int flag_teste_concluido;
        while (!fantasma_esquerdo_recebido || !fantasma_direito_recebido) {
            if (!fantasma_esquerdo_recebido) {
                MPI_Test(&requisicoes_recebimento[0], &flag_teste_concluido, MPI_STATUS_IGNORE);
                if (flag_teste_concluido) fantasma_esquerdo_recebido = 1;
            }
            if (!fantasma_direito_recebido) {
                int indice_req_direita = (rank_processo > 0) ? 1 : 0;
                MPI_Test(&requisicoes_recebimento[indice_req_direita], &flag_teste_concluido, MPI_STATUS_IGNORE);
                if (flag_teste_concluido) fantasma_direito_recebido = 1;
            }
        }

        // 4. Calcula os pontos de fronteira.
        if (contagem_pontos_locais > 0) {
            if (rank_processo > 0) {
                nova_temperatura[1] = temperatura[1] + CONSTANTE_DE_DIFUSAO * (temperatura[0] - 2.0 * temperatura[1] + temperatura[2]);
            }
            if (rank_processo < num_processos - 1) {
                nova_temperatura[contagem_pontos_locais] = temperatura[contagem_pontos_locais] + CONSTANTE_DE_DIFUSAO * (temperatura[contagem_pontos_locais - 1] - 2.0 * temperatura[contagem_pontos_locais] + temperatura[contagem_pontos_locais + 1]);
            }
        }
        
        // Garante que os envios terminaram antes de iniciar a próxima iteração.
        MPI_Waitall(contagem_envio, requisicoes_envio, MPI_STATUSES_IGNORE);

        double* ponteiro_temp = temperatura;
        temperatura = nova_temperatura;
        nova_temperatura = ponteiro_temp;
    }

    double tempo_fim = MPI_Wtime();

    if (rank_processo == 0) {
        printf("Versão Não-Bloqueante (Test): Tempo de execução = %f segundos\n", tempo_fim - tempo_inicio);
    }

    free(temperatura);
    free(nova_temperatura);
    MPI_Finalize();
    return 0;
}