#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NUMERO_DE_PASSOS 2000

// Funções auxiliares para alocar e liberar a memória da grade
double** alocar_grade(int largura, int altura) {
    double *dados = (double*)malloc(largura * altura * sizeof(double));
    double **grade = (double**)malloc(largura * sizeof(double*));
    for (int i = 0; i < largura; i++) {
        grade[i] = &(dados[i * altura]);
    }
    return grade;
}

void liberar_grade(double** grade) {
    free(grade[0]);
    free(grade);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <TAMANHO_DA_GRADE>\n", argv[0]);
        return 1;
    }
    int LARGURA = atoi(argv[1]);
    int ALTURA = LARGURA;

    // Alocação das grades
    double **u = alocar_grade(LARGURA, ALTURA);
    double **v = alocar_grade(LARGURA, ALTURA);
    double **u_novo = alocar_grade(LARGURA, ALTURA);
    double **v_novo = alocar_grade(LARGURA, ALTURA);
    
    // Condição Inicial
    #pragma omp parallel for
    for (int i = 0; i < LARGURA; i++) {
        for (int j = 0; j < ALTURA; j++) {
            u[i][j] = 1.0; v[i][j] = 0.0;
            double dx = i - LARGURA/2.0, dy = j - ALTURA/2.0;
            double dist = sqrt(dx*dx + dy*dy);
            if (dist < (LARGURA / 25.0)) {
                u[i][j] += 2.0 * exp(-dist*dist/(LARGURA/5.0));
                v[i][j] += 1.5 * exp(-dist*dist/(LARGURA/5.0));
            }
        }
    }
    
    double tempo_inicial = omp_get_wtime();
    
    #pragma omp parallel
    {
        for (int passo = 0; passo < NUMERO_DE_PASSOS; passo++) {
            
            #pragma omp for collapse(2) schedule(static)
            for (int i = 1; i < LARGURA-1; i++) {
                for (int j = 1; j < ALTURA-1; j++) {
                    double d2u_dx2 = (u[i+1][j] - 2.0*u[i][j] + u[i-1][j]);
                    double d2u_dy2 = (u[i][j+1] - 2.0*u[i][j] + u[i][j-1]);
                    double d2v_dx2 = (v[i+1][j] - 2.0*v[i][j] + v[i-1][j]);
                    double d2v_dy2 = (v[i][j+1] - 2.0*v[i][j] + v[i][j-1]);
                    
                    u_novo[i][j] = u[i][j] + (0.001 * 0.01) * (d2u_dx2 + d2u_dy2);
                    v_novo[i][j] = v[i][j] + (0.001 * 0.01) * (d2v_dx2 + d2v_dy2);
                }
            }
            
            #pragma omp for
            for (int i = 0; i < LARGURA; i++) {
                u_novo[i][0] = u_novo[i][ALTURA-2];
                u_novo[i][ALTURA-1] = u_novo[i][1];
                v_novo[i][0] = v_novo[i][ALTURA-2];
                v_novo[i][ALTURA-1] = v_novo[i][1];
            }

            #pragma omp for
            for (int j = 0; j < ALTURA; j++) {
                u_novo[0][j] = u_novo[LARGURA-2][j];
                u_novo[LARGURA-1][j] = u_novo[1][j];
                v_novo[0][j] = v_novo[LARGURA-2][j];
                v_novo[LARGURA-1][j] = v_novo[1][j]; 
            }
            
            #pragma omp single
            {
                double **temp_u = u;
                u = u_novo;
                u_novo = temp_u;
                double **temp_v = v;
                v = v_novo;
                v_novo = temp_v;
            }
        }
    }
    
    double tempo_final = omp_get_wtime();
    printf("%.6f\n", tempo_final - tempo_inicial);
    
    liberar_grade(u);
    liberar_grade(v);
    liberar_grade(u_novo);
    liberar_grade(v_novo);
    
    return 0;
}