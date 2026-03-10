// ns_serial_snapshots.c — gera 6 snapshots para plotagem
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NX 256
#define NY 256

// tempo total = NT*DT. Ajustei para a difusão ficar visível.
#define NT 10000
#define DT 0.02     // passo de tempo
#define NU 0.20     // viscosidade (maior -> difunde mais rápido)

// largura da gaussiana: exp(-r^2 / GAUSS_VAR). Menor -> mais estreita -> difunde mais rápido.
#define GAUSS_VAR 25.0

// checkpoints para as 6 figuras (iguais ao Weber em espírito)
static const int CP[6] = {0, 2000, 4000, 6000, 8000, 10000};

static void init(double **u) {
    int cx=NX/2, cy=NY/2;
    for (int i=0;i<NX;i++){
        for (int j=0;j<NY;j++){
            u[i][j] = 1.0;
            double dx=i-cx, dy=j-cy, r2=dx*dx+dy*dy;
            double g = exp(-r2/GAUSS_VAR);
            u[i][j] += 2.0*g;                // pico ≈ 3.0 no centro
        }
    }
}

static void periodic(double **a){
    for (int i=0;i<NX;i++){ a[i][0]=a[i][NY-2]; a[i][NY-1]=a[i][1]; }
    for (int j=0;j<NY;j++){ a[0][j]=a[NX-2][j]; a[NX-1][j]=a[1][j]; }
}

static void dump_csv(const char* name, double **A){
    FILE* f=fopen(name,"w");
    for (int i=0;i<NX;i++){
        for (int j=0;j<NY;j++){
            fprintf(f,"%.6f%s", A[i][j], (j==NY-1) ? "\n" : ",");
        }
    }
    fclose(f);
}

int main(){
    // alocação simples (ponteiro de ponteiro mesmo)
    double **u=(double**)malloc(NX*sizeof(double*));
    double **un=(double**)malloc(NX*sizeof(double*));
    for(int i=0;i<NX;i++){ u[i]=(double*)malloc(NY*sizeof(double)); un[i]=(double*)malloc(NY*sizeof(double)); }

    init(u);
    periodic(u); // garante bordas válidas antes do 1º passo

    // snapshot inicial
    dump_csv("snap_t00000.csv", u);

    int next_cp_idx = 1; // já salvamos t=0
    for (int t=1; t<=NT; t++){
        for (int i=1;i<NX-1;i++){
            for (int j=1;j<NY-1;j++){
                double lap = (u[i+1][j]+u[i-1][j]+u[i][j+1]+u[i][j-1]-4.0*u[i][j]);
                un[i][j] = u[i][j] + DT*NU*lap;
            }
        }
        periodic(un);
        // swap
        double **tmp=u; u=un; un=tmp;

        if (next_cp_idx < 6 && t == CP[next_cp_idx]){
            char fname[64];
            snprintf(fname,sizeof(fname),"snap_t%05d.csv", t);
            dump_csv(fname, u);
            next_cp_idx++;
        }
    }

    for(int i=0;i<NX;i++){ free(u[i]); free(un[i]); }
    free(u); free(un);
    return 0;
}
