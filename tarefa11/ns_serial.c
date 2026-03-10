#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> // Usado apenas para o timer omp_get_wtime() para consistência

// --- Parâmetros da Simulação ---
#define LARGURA_GRID 512
#define ALTURA_GRID 512
#define NUMERO_DE_PASSOS 1000
#define PASSO_DE_TEMPO 0.001
#define VISCOSIDADE 0.01

// Função para inicializar os campos de velocidade (u, v) da grade.
void inicializar_velocidades(double **velocidade_x, double **velocidade_y, int usar_perturbacao) {
    int centro_x = LARGURA_GRID / 2;
    int centro_y = ALTURA_GRID / 2;

    for (int x = 0; x < LARGURA_GRID; x++) {
        for (int y = 0; y < ALTURA_GRID; y++) {
            velocidade_x[x][y] = 0.0;
            velocidade_y[x][y] = 0.0;

            // Adiciona uma perturbação gaussiana no centro para iniciar o movimento.
            if (usar_perturbacao) {
                double dx = x - centro_x;
                double dy = y - centro_y;
                double gauss = exp(-(dx * dx + dy * dy) / 100.0);
                velocidade_x[x][y] += 2.0 * gauss;
                velocidade_y[x][y] += 1.5 * gauss;
            }
        }
    }
}

// Aplica condições de contorno periódicas.
void aplicar_contorno(double **proxima_velocidade_x, double **proxima_velocidade_y) {
    // Contorno vertical (em Y)
    for (int x = 0; x < LARGURA_GRID; x++) {
        proxima_velocidade_x[x][0] = proxima_velocidade_x[x][ALTURA_GRID - 2];
        proxima_velocidade_x[x][ALTURA_GRID - 1] = proxima_velocidade_x[x][1];
        proxima_velocidade_y[x][0] = proxima_velocidade_y[x][ALTURA_GRID - 2];
        proxima_velocidade_y[x][ALTURA_GRID - 1] = proxima_velocidade_y[x][1];
    }
    // Contorno horizontal (em X)
    for (int y = 0; y < ALTURA_GRID; y++) {
        proxima_velocidade_x[0][y] = proxima_velocidade_x[LARGURA_GRID - 2][y];
        proxima_velocidade_x[LARGURA_GRID - 1][y] = proxima_velocidade_x[1][y];
        proxima_velocidade_y[0][y] = proxima_velocidade_y[LARGURA_GRID - 2][y];
        proxima_velocidade_y[LARGURA_GRID - 1][y] = proxima_velocidade_y[1][y];
    }
}

int main(int argc, char **argv) {
    // Alocação de memória para as grades de velocidade
    double **velocidade_x = (double**)malloc(LARGURA_GRID * sizeof(double*));
    double **velocidade_y = (double**)malloc(LARGURA_GRID * sizeof(double*));
    double **proxima_velocidade_x = (double**)malloc(LARGURA_GRID * sizeof(double*));
    double **proxima_velocidade_y = (double**)malloc(LARGURA_GRID * sizeof(double*));

    for(int i = 0; i < LARGURA_GRID; i++) {
        velocidade_x[i] = (double*)malloc(ALTURA_GRID * sizeof(double));
        velocidade_y[i] = (double*)malloc(ALTURA_GRID * sizeof(double));
        proxima_velocidade_x[i] = (double*)malloc(ALTURA_GRID * sizeof(double));
        proxima_velocidade_y[i] = (double*)malloc(ALTURA_GRID * sizeof(double));
    }

    inicializar_velocidades(velocidade_x, velocidade_y, 1);

    // Usa o timer do OpenMP para medir o tempo
    double tempo_inicio = omp_get_wtime();

    // Loop principal da simulação que avança no tempo
    for (int passo_atual = 0; passo_atual < NUMERO_DE_PASSOS; passo_atual++) {
        
        // Calcula o termo de difusão (viscosidade) para cada ponto interno da grade
        for (int x = 1; x < LARGURA_GRID - 1; x++) {
            for (int y = 1; y < ALTURA_GRID - 1; y++) {
                // Aproximação do Laplaciano usando diferenças finitas
                double laplaciano_x = velocidade_x[x+1][y] + velocidade_x[x-1][y] + velocidade_x[x][y+1] + velocidade_x[x][y-1] - 4.0 * velocidade_x[x][y];
                double laplaciano_y = velocidade_y[x+1][y] + velocidade_y[x-1][y] + velocidade_y[x][y+1] + velocidade_y[x][y-1] - 4.0 * velocidade_y[x][y];
                
                // Atualiza a velocidade para o próximo passo de tempo
                proxima_velocidade_x[x][y] = velocidade_x[x][y] + PASSO_DE_TEMPO * VISCOSIDADE * laplaciano_x;
                proxima_velocidade_y[x][y] = velocidade_y[x][y] + PASSO_DE_TEMPO * VISCOSIDADE * laplaciano_y;
            }
        }
        
        aplicar_contorno(proxima_velocidade_x, proxima_velocidade_y);

        // Troca os ponteiros: a grade "próxima" se torna a "atual" para a próxima iteração
        double **ponteiro_temporario = velocidade_x;
        velocidade_x = proxima_velocidade_x;
        proxima_velocidade_x = ponteiro_temporario;
        
        ponteiro_temporario = velocidade_y;
        velocidade_y = proxima_velocidade_y;
        proxima_velocidade_y = ponteiro_temporario;
    }

    double tempo_fim = omp_get_wtime();
    printf("[SERIAL] Tempo de execução: %.6f segundos\n", tempo_fim - tempo_inicio);

    // Liberação da memória alocada
    for (int i = 0; i < LARGURA_GRID; i++) {
        free(velocidade_x[i]);
        free(velocidade_y[i]);
        free(proxima_velocidade_x[i]);
        free(proxima_velocidade_y[i]);
    }
    free(velocidade_x);
    free(velocidade_y);
    free(proxima_velocidade_x);
    free(proxima_velocidade_y);

    return 0;
}