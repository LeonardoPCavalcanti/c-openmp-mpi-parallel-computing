#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// --- Parâmetros da Simulação ---
#define LARGURA_GRID 512
#define ALTURA_GRID 512
#define NUMERO_DE_PASSOS 10000
#define PASSO_DE_TEMPO 0.001
#define VISCOSIDADE 0.01

// Função para inicializar os campos de velocidade (paralelizada com OpenMP)
void inicializar_velocidades(double **velocidade_x, double **velocidade_y, int usar_perturbacao){
    int centro_x = LARGURA_GRID / 2;
    int centro_y = ALTURA_GRID / 2;
    
    // Paraleliza os dois laços aninhados para acelerar a inicialização
    #pragma omp parallel for collapse(2)
    for (int x = 0; x < LARGURA_GRID; x++){
        for (int y = 0; y < ALTURA_GRID; y++){
            velocidade_x[x][y] = 1.0;
            velocidade_y[x][y] = 0.0;
            if (usar_perturbacao){
                double dx = x - centro_x;
                double dy = y - centro_y;
                double gauss = exp(-(dx * dx + dy * dy) / 100.0);
                velocidade_x[x][y] += 2.0 * gauss;
                velocidade_y[x][y] += 1.5 * gauss;
            }
        }
    }
}

// Aplica condições de contorno periódicas (paralelizada com OpenMP)
void aplicar_contorno(double **proxima_velocidade_x, double **proxima_velocidade_y){
    #pragma omp parallel for
    for (int x = 0; x < LARGURA_GRID; x++){
        proxima_velocidade_x[x][0] = proxima_velocidade_x[x][ALTURA_GRID - 2];
        proxima_velocidade_x[x][ALTURA_GRID - 1] = proxima_velocidade_x[x][1];
        proxima_velocidade_y[x][0] = proxima_velocidade_y[x][ALTURA_GRID - 2];
        proxima_velocidade_y[x][ALTURA_GRID - 1] = proxima_velocidade_y[x][1];
    }
    #pragma omp parallel for
    for (int y = 0; y < ALTURA_GRID; y++){
        proxima_velocidade_x[0][y] = proxima_velocidade_x[LARGURA_GRID - 2][y];
        proxima_velocidade_x[LARGURA_GRID - 1][y] = proxima_velocidade_x[1][y];
        proxima_velocidade_y[0][y] = proxima_velocidade_y[LARGURA_GRID - 2][y];
        proxima_velocidade_y[LARGURA_GRID - 1][y] = proxima_velocidade_y[1][y];
    }
}

int main(int argc, char **argv){
    int usar_perturbacao = (argc > 1) ? atoi(argv[1]) : 1;

    // Alocação de memória
    double **velocidade_x = (double**)malloc(LARGURA_GRID * sizeof(double*));
    double **velocidade_y = (double**)malloc(LARGURA_GRID * sizeof(double*));
    double **proxima_velocidade_x = (double**)malloc(LARGURA_GRID * sizeof(double*));
    double **proxima_velocidade_y = (double**)malloc(LARGURA_GRID * sizeof(double*));
    for (int x = 0; x < LARGURA_GRID; x++){
        velocidade_x[x] = (double*)malloc(ALTURA_GRID * sizeof(double));
        velocidade_y[x] = (double*)malloc(ALTURA_GRID * sizeof(double));
        proxima_velocidade_x[x] = (double*)malloc(ALTURA_GRID * sizeof(double));
        proxima_velocidade_y[x] = (double*)malloc(ALTURA_GRID * sizeof(double));
    }

    inicializar_velocidades(velocidade_x, velocidade_y, usar_perturbacao);

    double tempo_inicio = omp_get_wtime();

    for (int passo_atual = 0; passo_atual < NUMERO_DE_PASSOS; passo_atual++){
        // Otimização principal: paraleliza o laço externo (em 'x').
        // Cada thread fica responsável por um bloco de linhas da grade.
        #pragma omp parallel for
        for (int x = 1; x < LARGURA_GRID - 1; x++){
            for (int y = 1; y < ALTURA_GRID - 1; y++){
                double laplaciano_x = (velocidade_x[x+1][y] + velocidade_x[x-1][y] + velocidade_x[x][y+1] + velocidade_x[x][y-1] - 4.0 * velocidade_x[x][y]);
                double laplaciano_y = (velocidade_y[x+1][y] + velocidade_y[x-1][y] + velocidade_y[x][y+1] + velocidade_y[x][y-1] - 4.0 * velocidade_y[x][y]);
                proxima_velocidade_x[x][y] = velocidade_x[x][y] + PASSO_DE_TEMPO * VISCOSIDADE * laplaciano_x;
                proxima_velocidade_y[x][y] = velocidade_y[x][y] + PASSO_DE_TEMPO * VISCOSIDADE * laplaciano_y;
            }
        }

        aplicar_contorno(proxima_velocidade_x, proxima_velocidade_y);

        // Troca de ponteiros para a próxima iteração
        double **ponteiro_temporario = velocidade_x; 
        velocidade_x = proxima_velocidade_x; 
        proxima_velocidade_x = ponteiro_temporario;
        
        ponteiro_temporario = velocidade_y; 
        velocidade_y = proxima_velocidade_y; 
        proxima_velocidade_y = ponteiro_temporario;
    }

    double tempo_fim = omp_get_wtime();
    printf("[OMP Simples] Tempo de execução: %.6f s\n", tempo_fim - tempo_inicio);

    // Liberação de memória
    for (int x = 0; x < LARGURA_GRID; x++){ 
        free(velocidade_x[x]); 
        free(velocidade_y[x]); 
        free(proxima_velocidade_x[x]); 
        free(proxima_velocidade_y[x]); 
    }
    free(velocidade_x); 
    free(velocidade_y); 
    free(proxima_velocidade_x); 
    free(proxima_velocidade_y);
    
    return 0;
}