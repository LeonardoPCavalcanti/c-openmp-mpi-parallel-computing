#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NX 512       // número de pontos no eixo X
#define NY 512       // número de pontos no eixo Y
#define NT 10000     // número de passos de tempo
#define DT 0.001     // passo de tempo
#define NU 0.01      // coeficiente de viscosidade

#ifndef CHUNK_SIZE
#define CHUNK_SIZE 0
#endif

static inline void applyPeriodicBC(double **nextVelX, double **nextVelY) {
    for (int ix = 0; ix < NX; ix++) {
        nextVelX[ix][0]    = nextVelX[ix][NY-2];
        nextVelX[ix][NY-1] = nextVelX[ix][1];
        nextVelY[ix][0]    = nextVelY[ix][NY-2];
        nextVelY[ix][NY-1] = nextVelY[ix][1];
    }
    for (int iy = 0; iy < NY; iy++) {
        nextVelX[0][iy]    = nextVelX[NX-2][iy];
        nextVelX[NX-1][iy] = nextVelX[1][iy];
        nextVelY[0][iy]    = nextVelY[NX-2][iy];
        nextVelY[NX-1][iy] = nextVelY[1][iy];
    }
}

int main(int argc, char **argv) {
    int usePerturbation = 1;
    if (argc > 1) usePerturbation = atoi(argv[1]);

    // Alocação 2D
    double **velX      = (double**)malloc(NX * sizeof(double*));
    double **velY      = (double**)malloc(NX * sizeof(double*));
    double **nextVelX  = (double**)malloc(NX * sizeof(double*));
    double **nextVelY  = (double**)malloc(NX * sizeof(double*));
    for (int ix = 0; ix < NX; ix++) {
        velX[ix]     = (double*)malloc(NY * sizeof(double));
        velY[ix]     = (double*)malloc(NY * sizeof(double));
        nextVelX[ix] = (double*)malloc(NY * sizeof(double));
        nextVelY[ix] = (double*)malloc(NY * sizeof(double));
    }

    // Inicialização (paralela)
    #pragma omp parallel for collapse(2)
    for (int ix = 0; ix < NX; ix++) {
        for (int iy = 0; iy < NY; iy++) {
            velX[ix][iy] = 1.0;
            velY[ix][iy] = 0.0;
            if (usePerturbation) {
                int cx = NX/2, cy = NY/2;
                double dx = ix - cx, dy = iy - cy;
                double g  = exp(-(dx*dx + dy*dy)/100.0);
                velX[ix][iy] += 2.0 * g;
                velY[ix][iy] += 1.5 * g;
            }
        }
    }

    double tStart = omp_get_wtime();

    for (int step = 0; step < NT; step++) {
        #pragma omp parallel
        {
            // Atualização principal
            #if defined(SCHED_STATIC)
                #pragma omp for collapse(2) schedule(static)
            #elif defined(SCHED_DYNAMIC)
                #if CHUNK_SIZE > 0
                    #pragma omp for collapse(2) schedule(dynamic, CHUNK_SIZE)
                #else
                    #pragma omp for collapse(2) schedule(dynamic)
                #endif
            #elif defined(SCHED_GUIDED)
                #if CHUNK_SIZE > 0
                    #pragma omp for collapse(2) schedule(guided, CHUNK_SIZE)
                #else
                    #pragma omp for collapse(2) schedule(guided)
                #endif
            #else
                #pragma omp for collapse(2)
            #endif
            for (int ix = 1; ix < NX-1; ix++) {
                for (int iy = 1; iy < NY-1; iy++) {
                    double laplaceX = (velX[ix+1][iy] + velX[ix-1][iy] +
                                       velX[ix][iy+1] + velX[ix][iy-1] - 4.0 * velX[ix][iy]);
                    double laplaceY = (velY[ix+1][iy] + velY[ix-1][iy] +
                                       velY[ix][iy+1] + velY[ix][iy-1] - 4.0 * velY[ix][iy]);

                    nextVelX[ix][iy] = velX[ix][iy] + DT * NU * laplaceX;
                    nextVelY[ix][iy] = velY[ix][iy] + DT * NU * laplaceY;
                }
            }

            // Bordas (mesma região paralela)
            #pragma omp for
            for (int ix = 0; ix < NX; ix++) {
                nextVelX[ix][0]    = nextVelX[ix][NY-2];
                nextVelX[ix][NY-1] = nextVelX[ix][1];
                nextVelY[ix][0]    = nextVelY[ix][NY-2];
                nextVelY[ix][NY-1] = nextVelY[ix][1];
            }
            #pragma omp for
            for (int iy = 0; iy < NY; iy++) {
                nextVelX[0][iy]    = nextVelX[NX-2][iy];
                nextVelX[NX-1][iy] = nextVelX[1][iy];
                nextVelY[0][iy]    = nextVelY[NX-2][iy];
                nextVelY[NX-1][iy] = nextVelY[1][iy];
            }
        } // barreira implícita

        // Troca de buffers
        double **tmp = velX; velX = nextVelX; nextVelX = tmp;
        tmp = velY; velY = nextVelY; nextVelY = tmp;
    }

    double tEnd = omp_get_wtime();

    #if defined(SCHED_STATIC)
        printf("[OPT collapse+static] tempo=%.6f s\n", tEnd - tStart);
    #elif defined(SCHED_DYNAMIC)
        printf("[OPT collapse+dynamic%s] tempo=%.6f s\n",
               CHUNK_SIZE > 0 ? " (chunk)" : "", tEnd - tStart);
    #elif defined(SCHED_GUIDED)
        printf("[OPT collapse+guided%s] tempo=%.6f s\n",
               CHUNK_SIZE > 0 ? " (chunk)" : "", tEnd - tStart);
    #else
        printf("[OPT collapse+default] tempo=%.6f s\n", tEnd - tStart);
    #endif

    for (int ix = 0; ix < NX; ix++) { free(velX[ix]); free(velY[ix]); free(nextVelX[ix]); free(nextVelY[ix]); }
    free(velX); free(velY); free(nextVelX); free(nextVelY);
    return 0;
}
