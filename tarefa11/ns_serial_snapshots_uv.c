// ns_serial_snapshots_uv.c — gera 6 snapshots de U e V (difusão viscosa 2D)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NX 256
#define NY 256
#define NT 10000
#define DT 0.02      // passo de tempo (estável e rápido para difundir)
#define NU 0.20      // viscosidade (alto o suficiente p/ ficar visível)
#define GAUSS_VAR 25.0  // exp(-r^2/GAUSS_VAR) — menor => perturbação mais “fina”

static const int CP[6] = {0, 2000, 4000, 6000, 8000, 10000};

static inline void dump_csv(const char* name, double **A){
    FILE* f=fopen(name,"w");
    for (int i=0;i<NX;i++){
        for (int j=0;j<NY;j++){
            fprintf(f,"%.6f%s", A[i][j], (j==NY-1) ? "\n" : ",");
        }
    }
    fclose(f);
}

static inline void periodic(double **a){
    for (int i=0;i<NX;i++){ a[i][0]=a[i][NY-2]; a[i][NY-1]=a[i][1]; }
    for (int j=0;j<NY;j++){ a[0][j]=a[NX-2][j]; a[NX-1][j]=a[1][j]; }
}

int main(void){
    // alocação
    double **u=(double**)malloc(NX*sizeof(double*));
    double **v=(double**)malloc(NX*sizeof(double*));
    double **un=(double**)malloc(NX*sizeof(double*));
    double **vn=(double**)malloc(NX*sizeof(double*));
    for(int i=0;i<NX;i++){
        u[i]=(double*)malloc(NY*sizeof(double));
        v[i]=(double*)malloc(NY*sizeof(double));
        un[i]=(double*)malloc(NY*sizeof(double));
        vn[i]=(double*)malloc(NY*sizeof(double));
    }

    // inicialização: campo base 1.0 + perturbações gaussianas (U maior que V)
    int cx=NX/2, cy=NY/2;
    for (int i=0;i<NX;i++){
        for (int j=0;j<NY;j++){
            double dx=i-cx, dy=j-cy, r2=dx*dx+dy*dy;
            double g = exp(-r2/GAUSS_VAR);
            u[i][j] = 1.0 + 2.0*g;   // pico ~3.0
            v[i][j] = 1.0 + 1.2*g;   // pico ~2.2 (só pra distinguir)
        }
    }
    periodic(u); periodic(v);

    // snapshots iniciais
    dump_csv("snap_u_t00000.csv", u);
    dump_csv("snap_v_t00000.csv", v);

    int next_cp = 1; // já salvamos t=0
    for (int t=1; t<=NT; t++){
        for (int i=1;i<NX-1;i++){
            for (int j=1;j<NY-1;j++){
                double lapu = (u[i+1][j]+u[i-1][j]+u[i][j+1]+u[i][j-1]-4.0*u[i][j]);
                double lapv = (v[i+1][j]+v[i-1][j]+v[i][j+1]+v[i][j-1]-4.0*v[i][j]);
                un[i][j] = u[i][j] + DT*NU*lapu;
                vn[i][j] = v[i][j] + DT*NU*lapv;
            }
        }
        periodic(un); periodic(vn);
        // swap
        double **tmp=u; u=un; un=tmp;
        tmp=v; v=vn; vn=tmp;

        if (next_cp < 6 && t == CP[next_cp]){
            char fnu[64], fnv[64];
            snprintf(fnu,sizeof(fnu),"snap_u_t%05d.csv", t);
            snprintf(fnv,sizeof(fnv),"snap_v_t%05d.csv", t);
            dump_csv(fnu, u);
            dump_csv(fnv, v);
            next_cp++;
        }
    }

    for(int i=0;i<NX;i++){ free(u[i]); free(v[i]); free(un[i]); free(vn[i]); }
    free(u); free(v); free(un); free(vn);
    return 0;
}
